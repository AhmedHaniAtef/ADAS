################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP_Layer/src/APP.c \
../APP_Layer/src/CAN_task.c \
../APP_Layer/src/Messages_Callbacks.c 

OBJS += \
./APP_Layer/src/APP.o \
./APP_Layer/src/CAN_task.o \
./APP_Layer/src/Messages_Callbacks.o 

C_DEPS += \
./APP_Layer/src/APP.d \
./APP_Layer/src/CAN_task.d \
./APP_Layer/src/Messages_Callbacks.d 


# Each subdirectory must supply rules for building sources it contributes
APP_Layer/src/%.o APP_Layer/src/%.su APP_Layer/src/%.cyclo: ../APP_Layer/src/%.c APP_Layer/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I"D:/studying/Graduation_Project/Baremetal/ADAS _MOVING/lib/inc" -I"D:/studying/Graduation_Project/Baremetal/ADAS _MOVING/lib" -I"D:/studying/Graduation_Project/Baremetal/ADAS _MOVING/ECU_Layer" -I"D:/studying/Graduation_Project/Baremetal/ADAS _MOVING/ECU_Layer/inc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/studying/Graduation_Project/Baremetal/ADAS _MOVING/APP_Layer/inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-APP_Layer-2f-src

clean-APP_Layer-2f-src:
	-$(RM) ./APP_Layer/src/APP.cyclo ./APP_Layer/src/APP.d ./APP_Layer/src/APP.o ./APP_Layer/src/APP.su ./APP_Layer/src/CAN_task.cyclo ./APP_Layer/src/CAN_task.d ./APP_Layer/src/CAN_task.o ./APP_Layer/src/CAN_task.su ./APP_Layer/src/Messages_Callbacks.cyclo ./APP_Layer/src/Messages_Callbacks.d ./APP_Layer/src/Messages_Callbacks.o ./APP_Layer/src/Messages_Callbacks.su

.PHONY: clean-APP_Layer-2f-src

