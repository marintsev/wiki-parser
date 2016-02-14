################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/attr.c \
../src/open_tag.c \
../src/parser.c \
../src/wiki-parser.c 

OBJS += \
./src/attr.o \
./src/open_tag.o \
./src/parser.o \
./src/wiki-parser.o 

C_DEPS += \
./src/attr.d \
./src/open_tag.d \
./src/parser.d \
./src/wiki-parser.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -Werror -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


