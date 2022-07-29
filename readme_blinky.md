blinky - Simple GPIO Sample Code
================================

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in `Software_Use_Agreement.rtf` in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------
This sample project demonstrates a simple application that:

 1. Uses TIMER0_STATES_GPIO as a toggling output signal. This GPIO toggles with Timer Handler.

 2. Uses SYSTICK_STATES_GPIO as toggling output signal. This GPIO toggles with SysTick Handler. 
 
 3. Uses GPIO0 to detect button press (by triggering a falling edge). Each
 button press disables or re-enables toggling on TIMER0_STATES_GPIO and SYSTICK_STATES_GPIO.
 
 TIMER0_STATES_GPIO and SYSTICK_STATES_GPIO values are defined in 
Project Properties -> C/C++ Build -> Settings -> Preprocessor.
 
 4. Prepares the blinky application by running CRCSign.exe in the post-build step to pass the 
 program ROM verification of the application vector table. This check uses a 
 CRC (CRC-CCITT) calculated over the vector table, and stored to the last word of the vector 
 table (which is verified to also be the word before the application reset vector handler). 
 The result of this CRC check is stored to the SYSCTRL->SYSCLK_CNT register before starting 
 the application, storing BOOTROM_ERR_NONE (0x0) if the CRC matches, and 
 BOOTROM_ERR_BAD_CRC (0x7) if the CRC does not.
 
The CRCSign.exe application located in the RTE/Device/RSL15 folder modifies the application, 
to replace the last entry prior to the application reset vector with a CRC (CRC-CCITT) 
calculated over the vector table. This CRC will be used by the program ROM in the CRC check over the 
application's vector table. 

The blinky application allocates an additional word at this location for the CRCSign.exe. 
To allocate a word in this location, a global variable is allocated in the application code using:

    uint32_t _CRC __attribute__((section(".reset")));

_Note: This global variable is guaranteed to be placed at the beginning of 
       the reset section, as the reset vector handler (also stored to this 
       section), is linked as part of the CMSIS library after application
       code has been placed._

The source code exists in `app.c`.

Hardware Requirements
---------------------
This application can be executed on an Evaluation and Development Board.

Importing a Project
-------------------
To import the sample code into your IDE workspace, refer to the 
*Getting Started Guide* for your IDE for more information.

Verification
------------
To verify that this application is functioning correctly, TIMER0_STATES_GPIO and SYSTICK_STATES_GPIO
continuously toggle every 200 ms and 100 ms, repectively. As well, every time
a falling edge is applied to GPIO0, toggling on TIMER0_STATES_GPIO and SYSTICK_STATES_GPIO is disabled or
re-enabled.

Debug Catch Mode 
---------------- 

If the device goes into Low Power Mode or resets quickly, it might be difficult to connect 
the device to a debugger. You can hold the `DEBUG_CATCH_GPIO` low to enter Debug Catch Mode, 
which holds the program execution in a `while` loop to make it easier to connect to the debugger. 

To make use of Debug Catch Mode: 

1. Connect the `DEBUG_CATCH_GPIO` to ground (press and hold the SW1 pushbutton on the 
   evaluation board). 
2. Press the RESET button (this restarts the application, then enters Debug Catch Mode, 
   which holds program execution in a `while` loop). 
3. Connect the debugger so that you can re-program or erase the flash. Disconnect `DEBUG_CATCH_GPIO` 
   from ground (releasing SW1 pushbutton on the evaluation board).

***
  Copyright (c) 2022 Semiconductor Components Industries, LLC (d/b/a
  onsemi), All Rights Reserved
