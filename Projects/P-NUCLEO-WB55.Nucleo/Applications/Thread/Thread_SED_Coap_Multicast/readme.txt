/**
  @page Thread_SED_Coap_Multicast application
  
  @verbatim
  ******************************************************************************
  * @file    Thread/Thread_SED_Coap_Multicast/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the Thread Coap Multicast application
  ******************************************************************************
  *
  * Copyright (c) 2019 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license 
  * SLA0044, the "License"; You may not use this file except in compliance with 
  * the License. You may obtain a copy of the License at:
  *                               www.st.com/SLA0044
  *
  ******************************************************************************
  @endverbatim

@par Application Description 

How to exchange a Coap message using the Thread protocol.

This application requires two STM32WB55xx boards.

In a Thread network, nodes are split into two forwarding roles: Router or End Device.
The Thread Leader is a Router that is responsible for managing the set of Routers in a 
Thread network. An End Device communicates primarily with a single Router. 

An MTD device can only have the following roles: 
 - End Device, Sleepy End Device
An FTD device can have the following roles: 
 - End Device, Sleepy End Device, REED, Router, Leader 

In our Application which uses two devices, one device will act as a Leader (Router) in FTD mode
and the other one will act as a SleepyEndDevice in MTD mode.

The device that will act as a Leader must be flashed with the FTD application: 
use the Thread FTD application Thread_FTD_Coap_Multicast. 
The other device that will act as a SleepyEndDevice can be flashed with this MTD application. 
 
After the reset of the 2 boards, one board (Device 1) will automatically reach the 
Leader mode (Green LED2 ON) and the other one (Device 2) will automatically reach the
Sleepy End Device mode (Red LED3 ON) after a few seconds.

At this stage, these two boards belong to the same Thread network and Device 2 will 
send every second a Coap request to Device 1 in order to light on/off its blue LED.

  ___________________________                       ___________________________
  |  Device 2 (MTD)         |                       | Device 1 (FTD)          |
  |_________________________|                       |_________________________|  
  |                         |                       |                         |
  |                         |                       |                         |
  |        Timer [1sec]     |======> COAP =========>|    BLUE LED ON/OFF      |
  |                         |    Resource "light"   |                         |
  |                         |    Mode : Multicast   |                         |
  |                         |                       |                         |
  |                         |                       |                         |
  ---------------------------                       ---------------------------
  | Role : SleepyEndDevice  |                       | Role : Leader           |
  |                         |                       |                         |
  | LED : Red               |                       | LED : Green             |
  |                         |                       |                         |
  |_________________________|                       |_________________________|


       
@par Keywords

COAP,Thread

@par Directory contents 
  
  - Thread/Thread_SED_Coap_Multicast/Core/Inc/app_common.h            Header for all modules with common definition
  - Thread/Thread_SED_Coap_Multicast/Core/Inc/app_conf.h              Parameters configuration file of the application 
  - Thread/Thread_SED_Coap_Multicast/Core/Inc/app_entry.h             Parameters configuration file of the application
  - Thread/Thread_SED_Coap_Multicast/STM32_WPAN/App/app_thread.h      Header for app_thread.c module
  - Thread/Thread_SED_Coap_Multicast/Core/Inc/hw_conf.h               Configuration file of the HW 
  - Thread/Thread_SED_Coap_Multicast/Core/Inc/main.h                  Header for main.c module
  - Thread/Thread_SED_Coap_Multicast/Core/Inc/stm_logging.h           Header for stm_logging.c module
  - Thread/Thread_SED_Coap_Multicast/Core/Inc/stm32wbxx_hal_conf.h    HAL configuration file
  - Thread/Thread_SED_Coap_Multicast/Core/Src/stm32wbxx_it.h          Interrupt header file
  - Thread/Thread_SED_Coap_Multicast/Core/Inc/system_infra.h          System infrastructure header file
  - Thread/Thread_SED_Coap_Multicast/Core/Inc/utilities_conf.h        Configuration file of the utilities
  - Thread/Thread_SED_Coap_Multicast/Core/Src/app_entry.c             Initialization of the application
  - Thread/Thread_SED_Coap_Multicast/STM32_WPAN/App/app_thread.c      Thread application implementation
  - Thread/Thread_SED_Coap_Multicast/STM32_WPAN/Target/hw_ipcc.c      IPCC Driver
  - Thread/Thread_SED_Coap_Multicast/Core/Src/stm32_lpm_if.c          Low Power Manager Interface
  - Thread/Thread_SED_Coap_Multicast/Core/Src/hw_uart.c               UART driver
  - Thread/Thread_SED_Coap_Multicast/Core/Src/main.c                  Main program
  - Thread/Thread_SED_Coap_Multicast/Core/Src/stm_logging.c           Logging module for traces
  - Thread/Thread_SED_Coap_Multicast/Core/Src/stm32xx_it.c            Interrupt handlers
  - Thread/Thread_SED_Coap_Multicast/Core/Src/system_stm32wbxx.c      stm32wbxx system source file


@par Hardware and Software environment

  - This application uses two STM32WB55xx devices (Nucleo board and Dongle)
  
  - This example has been tested with an STMicroelectronics STM32WB55 Nucleo or Dongle
    board and can be easily tailored to any other supported device 
    and development board.
    
  - On STM32WB55RG_Nucleo, the jumpers must be configured as described
    in this section. Starting from the top left position up to the bottom 
    right position, the jumpers on the Board must be set as follows:

     CN11:    GND         [OFF]
     JP4:     VDDRF       [ON]
     JP6:     VC0         [ON]
     JP2:     +3V3        [ON] 
     JP1:     USB_STL     [ON]   All others [OFF]
     CN12:    GND         [OFF]
     CN7:     <All>       [OFF]
     JP3:     VDD_MCU     [ON]
     JP5:     GND         [OFF]  All others [ON]
     CN10:    <All>       [OFF]


@par How to use it ? 

For Device 1 (FTD), this application requests having the stm32wb5x_Thread_FTD_fw.bin binary flashed on the Wireless Coprocessor.
For Device 2 (MTD), this application requests having the stm32wb5x_Thread_MTD_fw.bin binary flashed on the Wireless Coprocessor.
If it is not the case, you need to use STM32CubeProgrammer to load the appropriate binary.
All available binaries are located under /Projects/STM32_Copro_Wireless_Binaries directory.
Refer to UM2237 to learn how to use/install STM32CubeProgrammer.
Refer to /Projects/STM32_Copro_Wireless_Binaries/ReleaseNote.html for the detailed procedure to change the
Wireless Coprocessor binary. 


In order to make the program work, you must do the following: 
 - Connect 2 STM32WB55xx boards to your PC

    One Board (Device 1) must be configured in FTD.
    -> Refer to the Thread/Thread_FTD_Coap_Multicast application example and follow the associated readme.txt

    The second Board (Device 2) must be configured in MTD 
    -> Open your preferred toolchain 
    -> Rebuild all files of Thread/Thread_SED_Coap_Multicast application 
         and load your image into the other target memory

 - Run the application
 
 If you want to control this application, you can directly send and
 retrieve Cli commands connecting an HyperTerminal with the ST_Link cable.
 (Refer to the Thread_Cli_Cmd application for more details) 
    
 If you want to get the traces in real time, you can connect an HyperTerminal 
 via an UART RS232 cable using the following connections:
  RXD : CN10(Pin35)   
  TXD : CN10(Pin37)
 
 For the Cli control and for the traces, the UART must be configured as follows:
    - BaudRate = 115200 baud  
    - Word Length = 8 Bits 
    - Stop Bit = 1 bit
    - Parity = none
    - Flow control = none

 Note for addressing the low power :
  ----------------------------------
 To set the Application in full power mode, it is requested to set the define 
 CFG_FULL_LOW_POWER to 1 inside the app_conf.h file and to recompile the application.
 In this case, the Application is optimized in terms of power consumption.
 In this configuration,in order to save power,the LEDs are always powered off,
 the debugger cannot be attached any more to the target and there are no more traces.


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */