/**
 * @file app.h
 * @brief Header file for main application
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

#ifndef APP_H_
#define APP_H_

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/
#include <hw.h>
#include <string.h>
#include <stdio.h>
#include <RTE_Device.h>
#include <gpio_driver.h>

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

/* GPIO definitions */

/* GPIO number that is used for Debug Catch Mode
 * to easily connect the device to the debugger */
#define DEBUG_CATCH_GPIO                7

/* GPIO number that is acting as LED of EVB */
#define TIMER0_STATES_GPIO              GREEN_LED
#define SYSTICK_STATES_GPIO             BLUE_LED

/* Set UART peripheral clock */
#define UART_CLK                        8000000

/* Set sensor clock */
#define SENSOR_CLK                      32768

/* Set user clock */
#define USER_CLK                        1000000

/* Enable/disable buck converter
 * Options: VCC_BUCK or VCC_LDO
 */
#define VCC_BUCK_LDO_CTRL               VCC_BUCK

/* To set Timer0 interrupt interval to 200 ms
 * Assumptions:
 *    - SYSCLK = 8 MHz
 *    - Slow Clock Prescale = 8
 *    - Timer0 Clock Source = SLOWCLK32
 *    - Timer0 Prescale = 8
 */
#define TIMER0_TIMEOUT_VAL_SETTING      781

/* To set SysTick interrupt interval to 100 ms (+/- 16 us)
 * Assumptions:
 *    - SYSCLK = 8 MHz
 *    - Slow Clock Prescale = 8
 *    - SysTick Clock Source = SLOWCLK32
 * Notes: 32 us/step: 3125 => 100 ms (+/- 16 us)
 */
#define SYSTICK_RELOAD_VAL_SETTING      3125

#define HW_GPIO_AOUT   2   ///< DIO used for exporting AOUT
#define HW_GPIO_DOUT   2   ///< DIO used for exporting DOUT

#define AOUT_VLAUE 	0x34
/*
        { 0x00, true,  "grounded" },
        { 0x01, true,  "VCC" },
        { 0x02, false, "BG_ptat_pmos" },
        { 0x03, false, "BG_vbe_ptat" },
        { 0x04, false, "BG_1uA_pmos" },
        { 0x05, false, "BG_50nA_pmos" },
        { 0x06, false, "BG_10nA_pmos" },
        { 0x07, true,  "BG_reg_flash" },
        { 0x09, true,  "BG_ref_0p75V_buffered" },
        { 0x0C, true,  "TEMP_voltage" },
        { 0x0D, false, "ADC_sensor_irange" },
        { 0x0E, true,  "LSAD_internal_vref" },
        { 0x0F, false, "Thermistor_current" },
        { 0x10, false, "PMT_PTAT_iref" },
        { 0x11, true,  "PMU_vddacs" },
        { 0x12, true,  "RC_internal" },
        { 0x13, false, "RC32_internal" },
        { 0x14, true,  "VDDA_switch" },
        { 0x15, true,  "VDDSYN_switch" },
        { 0x16, true,  "VDDRF_switch" },
        { 0x17, true,  "VDDT_switch" },
        { 0x18, true,  "VDDCCAO_switch" },
        { 0x19, true,  "VDDSENSOR_switch" },
        { 0x1A, true,  "VDDM" },
        { 0x1B, true,  "VDDC" },
        { 0x1C, true,  "VDDPA" },
        { 0x1D, false, "VDDPA_current_sensing" },
        { 0x1E, true,  "Flash1_TM0" },
        { 0x1F, true,  "Flash0_TM0" },
        { 0x20, true,  "VDDCP" },
        { 0x21, true,  "VDDRF" },
        { 0x22, true,  "VDDIF" },
        { 0x23, true,  "VDDFLASH" },
        { 0x24, false, "HIZ_leakage" },
        { 0x25, false, "RC32_NTC" },
        { 0x29, true,  "ADC_sensor_pulse_signal" },
        { 0x2A, true,  "SENSOR_det_active_signal" },
        { 0x2B, true,  "BG_ready" },
        { 0x2C, true,  "VDDRF_ready" },
        { 0x2D, true,  "VDDC_ready" },
        { 0x2E, true,  "VDDM_ready" },
        { 0x2F, true,  "VDDCP_ready" },
        { 0x30, true,  "VDDIF_ready" },
        { 0x31, true,  "VDDFLASH_ready" },
        { 0x32, true,  "CLK_DET_present" },
        { 0x33, true,  "XTAL_OK" },
        { 0x34, true,  "XTAL_CLK" },
        { 0x35, true,  "RC32_CLK" },
        { 0x36, true,  "DC-DC_activated" },
        { 0x37, true,  "DC-DC_overload" },
        { 0x38, true,  "DC-DC_vcc_ready" },
        { 0x39, true,  "NFC_wakeup_signal" },
        { 0x3A, true,  "NFC_field0" },
        { 0x3B, true,  "NFC_field1" },
        { 0x3C, true,  "NFC_field2" },
        { 0x3D, true,  "NFC_field3" },
        { 0xFF, false, NULL }
*/

/* ----------------------------------------------------------------------------
 * Global variables and types
 * --------------------------------------------------------------------------*/
extern DRIVER_GPIO_t Driver_GPIO;

/* ----------------------------------------------------------------------------
 * Function prototypes
 * --------------------------------------------------------------------------*/
/**
 * @brief   Toggle the global toggle status flag.
 */
extern void Button_EventCallback(uint32_t event);

/**
 * @brief    Timer0 Interrupt Handler
 */
extern void TIMER0_IRQHandler(void);

/**
 * @brief    SysTick Interrupt Handler
 */
extern void SysTick_Handler(void);

/**
 * @brief   Set AOUT function
 */
extern void hw_set_aout(const uint8_t value);

/**
 * @brief    Read Modify Write function
 */
extern void util_rmw(volatile uint32_t *addr, const uint32_t mask, const uint32_t set);


/**
 * @brief Initialize the system, including GPIOs and interrupts.
 * @return Zero
 */
void Initialize(void);

/**
 * @brief The main entry point for the program
 */
int main(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H_ */
