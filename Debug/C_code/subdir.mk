################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../C_code/client.o \
../C_code/common.o \
../C_code/server.o 

C_SRCS += \
../C_code/client.c \
../C_code/common.c \
../C_code/server.c 

OBJS += \
./C_code/client.o \
./C_code/common.o \
./C_code/server.o 

C_DEPS += \
./C_code/client.d \
./C_code/common.d \
./C_code/server.d 


# Each subdirectory must supply rules for building sources it contributes
C_code/%.o: ../C_code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


