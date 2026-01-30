################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/MMWAVE/mmwave.c 

OBJS += \
./Drivers/MMWAVE/mmwave.o 

C_DEPS += \
./Drivers/MMWAVE/mmwave.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/MMWAVE/%.o Drivers/MMWAVE/%.su Drivers/MMWAVE/%.cyclo: ../Drivers/MMWAVE/%.c Drivers/MMWAVE/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32C011xx -c -I../Core/Inc -I"C:/Users/nhrob/embedded/desk-timer-tracker/desk-time-tracker/Drivers/MMWAVE" -I"C:/Users/nhrob/embedded/desk-timer-tracker/desk-time-tracker/Drivers/SSD1306" -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-MMWAVE

clean-Drivers-2f-MMWAVE:
	-$(RM) ./Drivers/MMWAVE/mmwave.cyclo ./Drivers/MMWAVE/mmwave.d ./Drivers/MMWAVE/mmwave.o ./Drivers/MMWAVE/mmwave.su

.PHONY: clean-Drivers-2f-MMWAVE

