################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Nick/pnl/repos/STM32CubeWB/Drivers/BSP/Adafruit_Shield/stm32_adafruit_lcd.c \
C:/Users/Nick/pnl/repos/STM32CubeWB/Drivers/BSP/Adafruit_Shield/stm32_adafruit_sd.c 

OBJS += \
./Drivers/BSP/Adafruit_Shield/stm32_adafruit_lcd.o \
./Drivers/BSP/Adafruit_Shield/stm32_adafruit_sd.o 

C_DEPS += \
./Drivers/BSP/Adafruit_Shield/stm32_adafruit_lcd.d \
./Drivers/BSP/Adafruit_Shield/stm32_adafruit_sd.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Adafruit_Shield/stm32_adafruit_lcd.o: C:/Users/Nick/pnl/repos/STM32CubeWB/Drivers/BSP/Adafruit_Shield/stm32_adafruit_lcd.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_STM32WBXX_NUCLEO -DDEBUG -DSTM32WB55xx -c -IC:/Users/Nick/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/CMSIS/Include -IC:/Users/Nick/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/STM32WBxx_HAL_Driver/Inc -IC:/Users/Nick/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -IC:/Users/Nick/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/CMSIS/Device/ST/STM32WBxx/Include -IC:/Users/Nick/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/Third_Party/FatFs/src -I"C:/Users/Nick/pnl/repos/STM32CubeWB/Projects/P-NUCLEO-WB55.Nucleo/Applications/FatFs/FatFs_uSD_Standalone/FATFS/App" -I"C:/Users/Nick/pnl/repos/STM32CubeWB/Projects/P-NUCLEO-WB55.Nucleo/Applications/FatFs/FatFs_uSD_Standalone/FATFS/Target" -I"C:/Users/Nick/pnl/repos/STM32CubeWB/Projects/P-NUCLEO-WB55.Nucleo/Applications/FatFs/FatFs_uSD_Standalone/Core/Inc" -I"C:/Users/Nick/pnl/repos/STM32CubeWB/Drivers/BSP/P-NUCLEO-WB55.Nucleo" -I"C:/Users/Nick/pnl/repos/STM32CubeWB/Drivers/BSP/Adafruit_Shield" -I"C:/Users/Nick/pnl/repos/STM32CubeWB/Drivers/BSP/Components" -I"C:/Users/Nick/pnl/repos/STM32CubeWB/Drivers/BSP/Components/Common" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/Adafruit_Shield/stm32_adafruit_lcd.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Adafruit_Shield/stm32_adafruit_sd.o: C:/Users/Nick/pnl/repos/STM32CubeWB/Drivers/BSP/Adafruit_Shield/stm32_adafruit_sd.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_STM32WBXX_NUCLEO -DDEBUG -DSTM32WB55xx -c -IC:/Users/Nick/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/CMSIS/Include -IC:/Users/Nick/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/STM32WBxx_HAL_Driver/Inc -IC:/Users/Nick/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -IC:/Users/Nick/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Drivers/CMSIS/Device/ST/STM32WBxx/Include -IC:/Users/Nick/STM32Cube/Repository/STM32Cube_FW_WB_V1.3.0/Middlewares/Third_Party/FatFs/src -I"C:/Users/Nick/pnl/repos/STM32CubeWB/Projects/P-NUCLEO-WB55.Nucleo/Applications/FatFs/FatFs_uSD_Standalone/FATFS/App" -I"C:/Users/Nick/pnl/repos/STM32CubeWB/Projects/P-NUCLEO-WB55.Nucleo/Applications/FatFs/FatFs_uSD_Standalone/FATFS/Target" -I"C:/Users/Nick/pnl/repos/STM32CubeWB/Projects/P-NUCLEO-WB55.Nucleo/Applications/FatFs/FatFs_uSD_Standalone/Core/Inc" -I"C:/Users/Nick/pnl/repos/STM32CubeWB/Drivers/BSP/P-NUCLEO-WB55.Nucleo" -I"C:/Users/Nick/pnl/repos/STM32CubeWB/Drivers/BSP/Adafruit_Shield" -I"C:/Users/Nick/pnl/repos/STM32CubeWB/Drivers/BSP/Components" -I"C:/Users/Nick/pnl/repos/STM32CubeWB/Drivers/BSP/Components/Common" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/Adafruit_Shield/stm32_adafruit_sd.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

