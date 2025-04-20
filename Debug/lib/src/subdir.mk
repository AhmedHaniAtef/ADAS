################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/src/KalmanFilter.c \
../lib/src/PID.c \
../lib/src/diff_functionalities.c \
../lib/src/my_list.c \
../lib/src/my_queue.c \
../lib/src/my_stack.c 

OBJS += \
./lib/src/KalmanFilter.o \
./lib/src/PID.o \
./lib/src/diff_functionalities.o \
./lib/src/my_list.o \
./lib/src/my_queue.o \
./lib/src/my_stack.o 

C_DEPS += \
./lib/src/KalmanFilter.d \
./lib/src/PID.d \
./lib/src/diff_functionalities.d \
./lib/src/my_list.d \
./lib/src/my_queue.d \
./lib/src/my_stack.d 


# Each subdirectory must supply rules for building sources it contributes
lib/src/%.o lib/src/%.su lib/src/%.cyclo: ../lib/src/%.c lib/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I"D:/studying/Graduation_Project/Baremetal/ADAS/lib/inc" -I"D:/studying/Graduation_Project/Baremetal/ADAS/lib" -I"D:/studying/Graduation_Project/Baremetal/ADAS/ECU_Layer" -I"D:/studying/Graduation_Project/Baremetal/ADAS/ECU_Layer/inc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/studying/Graduation_Project/Baremetal/ADAS/APP_Layer/inc" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lib-2f-src

clean-lib-2f-src:
	-$(RM) ./lib/src/KalmanFilter.cyclo ./lib/src/KalmanFilter.d ./lib/src/KalmanFilter.o ./lib/src/KalmanFilter.su ./lib/src/PID.cyclo ./lib/src/PID.d ./lib/src/PID.o ./lib/src/PID.su ./lib/src/diff_functionalities.cyclo ./lib/src/diff_functionalities.d ./lib/src/diff_functionalities.o ./lib/src/diff_functionalities.su ./lib/src/my_list.cyclo ./lib/src/my_list.d ./lib/src/my_list.o ./lib/src/my_list.su ./lib/src/my_queue.cyclo ./lib/src/my_queue.d ./lib/src/my_queue.o ./lib/src/my_queue.su ./lib/src/my_stack.cyclo ./lib/src/my_stack.d ./lib/src/my_stack.o ./lib/src/my_stack.su

.PHONY: clean-lib-2f-src

