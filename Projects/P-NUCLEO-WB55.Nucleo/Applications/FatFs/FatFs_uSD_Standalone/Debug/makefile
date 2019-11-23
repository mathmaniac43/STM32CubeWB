################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/FatFs/subdir.mk
-include FATFS/Target/subdir.mk
-include FATFS/App/subdir.mk
-include Drivers/STM32WBxx_HAL_Driver/subdir.mk
-include Drivers/BSP/P-NUCLEO-WB55.Nucleo/subdir.mk
-include Drivers/BSP/Components/subdir.mk
-include Drivers/BSP/Adafruit_Shield/subdir.mk
-include Core/Src/subdir.mk
-include Application/User/subdir.mk
-include Application/Startup/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
FatFs_uSD_Standalone.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
FatFs_uSD_Standalone.list \


# All Target
all: FatFs_uSD_Standalone.elf secondary-outputs

# Tool invocations
FatFs_uSD_Standalone.elf: $(OBJS) $(USER_OBJS) C:\Users\Nick\pnl\repos\STM32CubeWB\Projects\P-NUCLEO-WB55.Nucleo\Applications\FatFs\FatFs_uSD_Standalone\STM32WB55RGVX_FLASH.ld
	arm-none-eabi-gcc -o "FatFs_uSD_Standalone.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Nick\pnl\repos\STM32CubeWB\Projects\P-NUCLEO-WB55.Nucleo\Applications\FatFs\FatFs_uSD_Standalone\STM32WB55RGVX_FLASH.ld" --specs=nosys.specs -Wl,-Map="FatFs_uSD_Standalone.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

FatFs_uSD_Standalone.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "FatFs_uSD_Standalone.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
