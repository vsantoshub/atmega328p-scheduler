################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
arduino_kernel.lss \

FLASH_IMAGE += \
arduino_kernel.hex \

EEPROM_IMAGE += \
arduino_kernel.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: arduino_kernel.elf secondary-outputs

# Tool invocations
arduino_kernel.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-g++  -s -Os -o"arduino_kernel.elf" $(OBJS) $(USER_OBJS) $(LIBS) -lm -Wl,-Map,arduino_kernel.map,--cref -mrelax -Wl,--gc-sections -L"/home/victor/workspace/eclipse_arduino/arduinoCore/Release" -mmcu=atmega328p
	@echo 'Finished building target: $@'
	@echo ' '

arduino_kernel.lss: arduino_kernel.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S arduino_kernel.elf  >"arduino_kernel.lss"
	@echo 'Finished building: $@'
	@echo ' '

arduino_kernel.hex: arduino_kernel.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex arduino_kernel.elf  "arduino_kernel.hex"
	@echo 'Finished building: $@'
	@echo ' '

arduino_kernel.eep: arduino_kernel.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex arduino_kernel.elf  "arduino_kernel.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: arduino_kernel.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega328p arduino_kernel.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(CXX_DEPS)$(S_UPPER_DEPS)$(C++_DEPS)$(ASM_DEPS)$(CC_DEPS)$(CPP_DEPS)$(C_UPPER_DEPS)$(SIZEDUMMY) arduino_kernel.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
