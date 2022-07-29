/**
 * @file app.c
 * @brief Main application source file
 *
 * @copyright @parblock
 * Copyright (c) 2022 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */

#include "app.h"

/* Global variables */
volatile uint8_t led_toggle_status;
DRIVER_GPIO_t *gpio;
uint32_t _CRC __attribute__((section(".reset")));

void TIMER0_IRQHandler(void)
{
    if (led_toggle_status == 1)
    {
        gpio->ToggleValue((GPIO_SEL_t)(TIMER0_STATES_GPIO));
    }
}

void SysTick_Handler(void)
{
    if (led_toggle_status == 1)
    {
        gpio->ToggleValue((GPIO_SEL_t)(SYSTICK_STATES_GPIO));
    }
}

void Button_EventCallback(uint32_t event)
{
    static uint8_t ignore_next_gpio_int = 0;
    if (ignore_next_gpio_int == 1)
    {
        ignore_next_gpio_int = 0;
    }
    else if (event == GPIO_EVENT_0_IRQ)
    {
        /* Button is pressed: Ignore next interrupt.
         * This is required to deal with the debounce circuit limitations. */
        ignore_next_gpio_int = 1;

        /* Invert the toggle status flag */
        if (led_toggle_status == 1)
        {
            led_toggle_status = 0;
        }
        else
        {
            led_toggle_status = 1;
        }
    }
}

/**
 * @brief Initialize the system, including GPIOs and interrupts.
 * @return Zero
 */
void Initialize(void)
{
    /* Interrupts off */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

    /* Disable all existing interrupts, clearing all pending source */
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();

	/* Debug Catch Mode
	 * If DEBUG_CATCH_GPIO is low, enter Debug Catch Mode which holds the program execution in a
	 * while loop to make it easier to connect to the debugger.
	 * We suggest retaining this feature during development.
	 */
    SYS_GPIO_CONFIG(DEBUG_CATCH_GPIO, (GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE |
                                       GPIO_WEAK_PULL_UP | GPIO_6X_DRIVE));

    while ((Sys_GPIO_Read(DEBUG_CATCH_GPIO)) == 0)
    {
        SYS_WATCHDOG_REFRESH();
    }

    /* Load default trim values. */
    uint32_t trim_error __attribute__((unused)) = SYS_TRIM_LOAD_DEFAULT();

    /* Enable the 48MHz XTAL */
    Sys_Clocks_XTALClkConfig(CK_DIV_1_6_PRESCALE_6_BYTE);

    /* Switch to (divided 48 MHz) oscillator clock, and update the
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

    /* Enable/disable buck converter */
#ifdef RSL15_CID
    Sys_ACS_WriteRegister(&ACS->VCC_CTRL, ((ACS->VCC_CTRL & ~VCC_BUCK) | VCC_BUCK_LDO_CTRL));
#else    /* ifdef RSL15_CID */
    ACS->VCC_CTRL = ((ACS->VCC_CTRL & ~VCC_BUCK) | VCC_BUCK_LDO_CTRL);
#endif    /* ifdef RSL15_CID */

    /* Configure clock dividers */
    Sys_Clocks_DividerConfig(UART_CLK, SENSOR_CLK, USER_CLK);

    /* Configure Baseband Controller Interface */
    BBIF->CTRL = (BB_CLK_ENABLE | BBCLK_DIVIDER_8);

    /* Interrupts back on */
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
}

/*
 * @brief Set AOUT to export analog signals
 * @return Zero
 */
void hw_set_aout(const uint8_t value)
{
    /* Shift value into mode bit position... */
    uint32_t test_aout = ((uint32_t) value) << ACS_AOUT_CTRL_TEST_AOUT_Pos;
    uint32_t test_mask = ACS_AOUT_CTRL_TEST_AOUT_Mask;

    /* Add the selected DIO... */
    test_aout |= (HW_GPIO_AOUT << ACS_AOUT_CTRL_AOUT_TO_GPIO_Pos);
    test_mask |= ACS_AOUT_CTRL_AOUT_TO_GPIO_Mask;

    /* Set the voltage/current select bit except for THERMISTOR_CURRENT... */
    test_aout |= ((value == AOUT_THERMISTOR_CURRENT ? 0 : 1) << ACS_AOUT_CTRL_AOUT_IOUT_SEL_TO_GPIO_Pos);
    test_mask |= (1 << ACS_AOUT_CTRL_AOUT_IOUT_SEL_TO_GPIO_Pos);

    /* Configure DIO for A-OUT... */
    SYS_GPIO_CONFIG(HW_GPIO_AOUT, GPIO_MODE_DISABLE | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_DOWN  | GPIO_6X_DRIVE);
    SYS_GPIO_CONFIG(HW_GPIO_AOUT, GPIO_MODE_DISABLE | GPIO_LPF_DISABLE | GPIO_NO_PULL  | GPIO_6X_DRIVE);

    /* Read/modify/write configuration register... */
    util_rmw((uint32_t *) ACS_AOUT_CTRL, test_mask, test_aout);
    return;
}

/**
 * Read/Modify/Write a 32-bit register..
 *
 * This routine performs a Read/Modify/Write operation on a 32-bit address.
 * The current value of the register is first AND'd with the 'mask' to clear
 * any fields being modified, and then OR'd with the 'set' to set the field
 * values.
 *
 * @param addr  [out] Pointer to 32-bit register to be modified.
 * @param mask  [in]  Bitmask of fields being modified.
 * @param set   [in]  Bits to be set within the bitmask.
 *
 * @return None.
 */
void util_rmw(volatile uint32_t *addr, const uint32_t mask, const uint32_t set)
{
    /* Bit to be set must exist within the mask... */
    uint32_t bitset = set & mask;

    /* READ the current value... */
    volatile uint32_t   value = *addr;

    /* MODIFY by ANDing with the mask and ORing with the set... */
    value &= (~mask);
    value |= bitset;

    /* WRITE the new value... */
    *addr = value;
    return;
}

/**
 * @brief The main entry point for the program
 */
int main(void)
{
    /*Initialize global variables */
    led_toggle_status = 1;

    /* Initialize the system */
    Initialize();

    hw_set_aout(AOUT_VLAUE);

#if 0
    /* Configure and start Timer 0 with a period of 200 ms */
    Sys_Timer_Config(TIMER0, TIMER_PRESCALE_8 , TIMER_FREE_RUN, TIMER0_TIMEOUT_VAL_SETTING);
    Sys_Timer_Start(TIMER0);
    NVIC_EnableIRQ(TIMER0_IRQn);

    /* Load and start SysTick Timer with a period of 100 ms (+/- 16 us) */
    SysTick->CTRL = 0;
    SysTick->LOAD = SYSTICK_RELOAD_VAL_SETTING;
    SysTick->VAL  = 0;
    SysTick->CTRL = ((0 << SysTick_CTRL_CLKSOURCE_Pos) |
                     (1 << SysTick_CTRL_TICKINT_Pos)   |
                     (1 << SysTick_CTRL_ENABLE_Pos));
    NVIC_EnableIRQ(SysTick_IRQn);

    /* Initialize GPIO structure */
    gpio = &Driver_GPIO;

    /* Initialize GPIO driver */
    gpio->Initialize(Button_EventCallback);
#endif

    /* Main application spin loop */
    while (1)
    {
        /* Refresh the watchdog timer */
        SYS_WATCHDOG_REFRESH();
    }

    /* Should never be reached */
    return 0;
}
