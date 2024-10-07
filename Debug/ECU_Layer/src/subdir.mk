################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ECU_Layer/src/ecu.c \
../ECU_Layer/src/motor.c 

OBJS += \
./ECU_Layer/src/ecu.o \
./ECU_Layer/src/motor.o 

C_DEPS += \
./ECU_Layer/src/ecu.d \
./ECU_Layer/src/motor.d 


# Each subdirectory must supply rules for building sources it contributes
ECU_Layer/src/%.o ECU_Layer/src/%.su ECU_Layer/src/%.cyclo: ../ECU_Layer/src/%.c ECU_Layer/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I"D:/studying/Graduation_Project/Baremetal/ADAS/ECU_Layer" -I"D:/studying/Graduation_Project/Baremetal/ADAS/ECU_Layer/inc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ECU_Layer-2f-src

clean-ECU_Layer-2f-src:
	-$(RM) ./ECU_Layer/src/ecu.cyclo ./ECU_Layer/src/ecu.d ./ECU_Layer/src/ecu.o ./ECU_Layer/src/ecu.su ./ECU_Layer/src/motor.cyclo ./ECU_Layer/src/motor.d ./ECU_Layer/src/motor.o ./ECU_Layer/src/motor.su

.PHONY: clean-ECU_Layer-2f-src

