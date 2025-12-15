################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Si7021\ TempSensor/Si7021_driver.c 

OBJS += \
./Si7021\ TempSensor/Si7021_driver.o 

C_DEPS += \
./Si7021\ TempSensor/Si7021_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Si7021\ TempSensor/Si7021_driver.o: ../Si7021\ TempSensor/Si7021_driver.c Si7021\ TempSensor/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/STM32-dev/Rx-Thermo/LoRa" -I"D:/STM32-dev/Rx-Thermo/Si7021 TempSensor" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Si7021 TempSensor/Si7021_driver.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Si7021-20-TempSensor

clean-Si7021-20-TempSensor:
	-$(RM) ./Si7021\ TempSensor/Si7021_driver.cyclo ./Si7021\ TempSensor/Si7021_driver.d ./Si7021\ TempSensor/Si7021_driver.o ./Si7021\ TempSensor/Si7021_driver.su

.PHONY: clean-Si7021-20-TempSensor

