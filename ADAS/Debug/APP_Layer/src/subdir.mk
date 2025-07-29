################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP_Layer/src/ACC_feature.c \
../APP_Layer/src/ALC_feature.c \
../APP_Layer/src/APK_feature.c \
../APP_Layer/src/APP.c \
../APP_Layer/src/BSD_feature.c \
../APP_Layer/src/CAN_task.c \
../APP_Layer/src/Control_task.c \
../APP_Layer/src/MPU_task.c \
../APP_Layer/src/Messages_Callbacks.c \
../APP_Layer/src/Monitoring_task.c \
../APP_Layer/src/Orientation_task.c \
../APP_Layer/src/TSR_feature.c \
../APP_Layer/src/Ultrasonic_task.c 

OBJS += \
./APP_Layer/src/ACC_feature.o \
./APP_Layer/src/ALC_feature.o \
./APP_Layer/src/APK_feature.o \
./APP_Layer/src/APP.o \
./APP_Layer/src/BSD_feature.o \
./APP_Layer/src/CAN_task.o \
./APP_Layer/src/Control_task.o \
./APP_Layer/src/MPU_task.o \
./APP_Layer/src/Messages_Callbacks.o \
./APP_Layer/src/Monitoring_task.o \
./APP_Layer/src/Orientation_task.o \
./APP_Layer/src/TSR_feature.o \
./APP_Layer/src/Ultrasonic_task.o 

C_DEPS += \
./APP_Layer/src/ACC_feature.d \
./APP_Layer/src/ALC_feature.d \
./APP_Layer/src/APK_feature.d \
./APP_Layer/src/APP.d \
./APP_Layer/src/BSD_feature.d \
./APP_Layer/src/CAN_task.d \
./APP_Layer/src/Control_task.d \
./APP_Layer/src/MPU_task.d \
./APP_Layer/src/Messages_Callbacks.d \
./APP_Layer/src/Monitoring_task.d \
./APP_Layer/src/Orientation_task.d \
./APP_Layer/src/TSR_feature.d \
./APP_Layer/src/Ultrasonic_task.d 


# Each subdirectory must supply rules for building sources it contributes
APP_Layer/src/%.o APP_Layer/src/%.su APP_Layer/src/%.cyclo: ../APP_Layer/src/%.c APP_Layer/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I"D:/studying/Graduation_Project/Baremetal/ADAS/lib/inc" -I"D:/studying/Graduation_Project/Baremetal/ADAS/lib" -I"D:/studying/Graduation_Project/Baremetal/ADAS/ECU_Layer" -I"D:/studying/Graduation_Project/Baremetal/ADAS/ECU_Layer/inc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/studying/Graduation_Project/Baremetal/ADAS/APP_Layer/inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-APP_Layer-2f-src

clean-APP_Layer-2f-src:
	-$(RM) ./APP_Layer/src/ACC_feature.cyclo ./APP_Layer/src/ACC_feature.d ./APP_Layer/src/ACC_feature.o ./APP_Layer/src/ACC_feature.su ./APP_Layer/src/ALC_feature.cyclo ./APP_Layer/src/ALC_feature.d ./APP_Layer/src/ALC_feature.o ./APP_Layer/src/ALC_feature.su ./APP_Layer/src/APK_feature.cyclo ./APP_Layer/src/APK_feature.d ./APP_Layer/src/APK_feature.o ./APP_Layer/src/APK_feature.su ./APP_Layer/src/APP.cyclo ./APP_Layer/src/APP.d ./APP_Layer/src/APP.o ./APP_Layer/src/APP.su ./APP_Layer/src/BSD_feature.cyclo ./APP_Layer/src/BSD_feature.d ./APP_Layer/src/BSD_feature.o ./APP_Layer/src/BSD_feature.su ./APP_Layer/src/CAN_task.cyclo ./APP_Layer/src/CAN_task.d ./APP_Layer/src/CAN_task.o ./APP_Layer/src/CAN_task.su ./APP_Layer/src/Control_task.cyclo ./APP_Layer/src/Control_task.d ./APP_Layer/src/Control_task.o ./APP_Layer/src/Control_task.su ./APP_Layer/src/MPU_task.cyclo ./APP_Layer/src/MPU_task.d ./APP_Layer/src/MPU_task.o ./APP_Layer/src/MPU_task.su ./APP_Layer/src/Messages_Callbacks.cyclo ./APP_Layer/src/Messages_Callbacks.d ./APP_Layer/src/Messages_Callbacks.o ./APP_Layer/src/Messages_Callbacks.su ./APP_Layer/src/Monitoring_task.cyclo ./APP_Layer/src/Monitoring_task.d ./APP_Layer/src/Monitoring_task.o ./APP_Layer/src/Monitoring_task.su ./APP_Layer/src/Orientation_task.cyclo ./APP_Layer/src/Orientation_task.d ./APP_Layer/src/Orientation_task.o ./APP_Layer/src/Orientation_task.su ./APP_Layer/src/TSR_feature.cyclo ./APP_Layer/src/TSR_feature.d ./APP_Layer/src/TSR_feature.o ./APP_Layer/src/TSR_feature.su ./APP_Layer/src/Ultrasonic_task.cyclo ./APP_Layer/src/Ultrasonic_task.d ./APP_Layer/src/Ultrasonic_task.o ./APP_Layer/src/Ultrasonic_task.su

.PHONY: clean-APP_Layer-2f-src

