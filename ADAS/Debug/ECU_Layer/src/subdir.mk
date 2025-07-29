################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ECU_Layer/src/CANSPI.c \
../ECU_Layer/src/Control.c \
../ECU_Layer/src/MCP2515.c \
../ECU_Layer/src/ecu.c \
../ECU_Layer/src/encoder.c \
../ECU_Layer/src/logic.c \
../ECU_Layer/src/monitoring.c \
../ECU_Layer/src/motor.c \
../ECU_Layer/src/mpu.c \
../ECU_Layer/src/qmc5883l.c \
../ECU_Layer/src/robot.c \
../ECU_Layer/src/ultrasonic.c 

OBJS += \
./ECU_Layer/src/CANSPI.o \
./ECU_Layer/src/Control.o \
./ECU_Layer/src/MCP2515.o \
./ECU_Layer/src/ecu.o \
./ECU_Layer/src/encoder.o \
./ECU_Layer/src/logic.o \
./ECU_Layer/src/monitoring.o \
./ECU_Layer/src/motor.o \
./ECU_Layer/src/mpu.o \
./ECU_Layer/src/qmc5883l.o \
./ECU_Layer/src/robot.o \
./ECU_Layer/src/ultrasonic.o 

C_DEPS += \
./ECU_Layer/src/CANSPI.d \
./ECU_Layer/src/Control.d \
./ECU_Layer/src/MCP2515.d \
./ECU_Layer/src/ecu.d \
./ECU_Layer/src/encoder.d \
./ECU_Layer/src/logic.d \
./ECU_Layer/src/monitoring.d \
./ECU_Layer/src/motor.d \
./ECU_Layer/src/mpu.d \
./ECU_Layer/src/qmc5883l.d \
./ECU_Layer/src/robot.d \
./ECU_Layer/src/ultrasonic.d 


# Each subdirectory must supply rules for building sources it contributes
ECU_Layer/src/%.o ECU_Layer/src/%.su ECU_Layer/src/%.cyclo: ../ECU_Layer/src/%.c ECU_Layer/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I"D:/studying/Graduation_Project/Baremetal/ADAS/lib/inc" -I"D:/studying/Graduation_Project/Baremetal/ADAS/lib" -I"D:/studying/Graduation_Project/Baremetal/ADAS/ECU_Layer" -I"D:/studying/Graduation_Project/Baremetal/ADAS/ECU_Layer/inc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/studying/Graduation_Project/Baremetal/ADAS/APP_Layer/inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ECU_Layer-2f-src

clean-ECU_Layer-2f-src:
	-$(RM) ./ECU_Layer/src/CANSPI.cyclo ./ECU_Layer/src/CANSPI.d ./ECU_Layer/src/CANSPI.o ./ECU_Layer/src/CANSPI.su ./ECU_Layer/src/Control.cyclo ./ECU_Layer/src/Control.d ./ECU_Layer/src/Control.o ./ECU_Layer/src/Control.su ./ECU_Layer/src/MCP2515.cyclo ./ECU_Layer/src/MCP2515.d ./ECU_Layer/src/MCP2515.o ./ECU_Layer/src/MCP2515.su ./ECU_Layer/src/ecu.cyclo ./ECU_Layer/src/ecu.d ./ECU_Layer/src/ecu.o ./ECU_Layer/src/ecu.su ./ECU_Layer/src/encoder.cyclo ./ECU_Layer/src/encoder.d ./ECU_Layer/src/encoder.o ./ECU_Layer/src/encoder.su ./ECU_Layer/src/logic.cyclo ./ECU_Layer/src/logic.d ./ECU_Layer/src/logic.o ./ECU_Layer/src/logic.su ./ECU_Layer/src/monitoring.cyclo ./ECU_Layer/src/monitoring.d ./ECU_Layer/src/monitoring.o ./ECU_Layer/src/monitoring.su ./ECU_Layer/src/motor.cyclo ./ECU_Layer/src/motor.d ./ECU_Layer/src/motor.o ./ECU_Layer/src/motor.su ./ECU_Layer/src/mpu.cyclo ./ECU_Layer/src/mpu.d ./ECU_Layer/src/mpu.o ./ECU_Layer/src/mpu.su ./ECU_Layer/src/qmc5883l.cyclo ./ECU_Layer/src/qmc5883l.d ./ECU_Layer/src/qmc5883l.o ./ECU_Layer/src/qmc5883l.su ./ECU_Layer/src/robot.cyclo ./ECU_Layer/src/robot.d ./ECU_Layer/src/robot.o ./ECU_Layer/src/robot.su ./ECU_Layer/src/ultrasonic.cyclo ./ECU_Layer/src/ultrasonic.d ./ECU_Layer/src/ultrasonic.o ./ECU_Layer/src/ultrasonic.su

.PHONY: clean-ECU_Layer-2f-src

