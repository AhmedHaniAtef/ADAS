################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/src/PID.c \
../lib/src/list.c \
../lib/src/queue.c \
../lib/src/stack.c 

OBJS += \
./lib/src/PID.o \
./lib/src/list.o \
./lib/src/queue.o \
./lib/src/stack.o 

C_DEPS += \
./lib/src/PID.d \
./lib/src/list.d \
./lib/src/queue.d \
./lib/src/stack.d 


# Each subdirectory must supply rules for building sources it contributes
lib/src/%.o lib/src/%.su lib/src/%.cyclo: ../lib/src/%.c lib/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I"D:/studying/Graduation_Project/Baremetal/ADAS/lib/inc" -I"D:/studying/Graduation_Project/Baremetal/ADAS/lib" -I"D:/studying/Graduation_Project/Baremetal/ADAS/ECU_Layer" -I"D:/studying/Graduation_Project/Baremetal/ADAS/ECU_Layer/inc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lib-2f-src

clean-lib-2f-src:
	-$(RM) ./lib/src/PID.cyclo ./lib/src/PID.d ./lib/src/PID.o ./lib/src/PID.su ./lib/src/list.cyclo ./lib/src/list.d ./lib/src/list.o ./lib/src/list.su ./lib/src/queue.cyclo ./lib/src/queue.d ./lib/src/queue.o ./lib/src/queue.su ./lib/src/stack.cyclo ./lib/src/stack.d ./lib/src/stack.o ./lib/src/stack.su

.PHONY: clean-lib-2f-src

