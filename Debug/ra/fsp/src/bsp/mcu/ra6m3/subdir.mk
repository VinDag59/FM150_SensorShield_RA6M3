################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/fsp/src/bsp/mcu/ra6m3/bsp_linker.c 

C_DEPS += \
./ra/fsp/src/bsp/mcu/ra6m3/bsp_linker.d 

CREF += \
FM150_SensorShield_RA6M3.cref 

OBJS += \
./ra/fsp/src/bsp/mcu/ra6m3/bsp_linker.o 

MAP += \
FM150_SensorShield_RA6M3.map 


# Each subdirectory must supply rules for building sources it contributes
ra/fsp/src/bsp/mcu/ra6m3/%.o: ../ra/fsp/src/bsp/mcu/ra6m3/%.c
	@echo 'Building file: $<'
	$(file > $@.in,-mcpu=cortex-m4 -mthumb -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -ffunction-sections -fdata-sections -fno-strict-aliasing -fmessage-length=0 -funsigned-char -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Waggregate-return -Wno-parentheses-equality -Wfloat-equal -g3 -std=c99 -fshort-enums -fno-unroll-loops -I"W:\\Workspace_2025-12\\FM150_SensorShield_RA6M3\\ra_gen" -I"." -I"W:\\Workspace_2025-12\\FM150_SensorShield_RA6M3\\ra_cfg\\fsp_cfg\\bsp" -I"W:\\Workspace_2025-12\\FM150_SensorShield_RA6M3\\ra_cfg\\fsp_cfg" -I"W:\\Workspace_2025-12\\FM150_SensorShield_RA6M3\\src" -I"W:\\Workspace_2025-12\\FM150_SensorShield_RA6M3\\ra\\fsp\\inc" -I"W:\\Workspace_2025-12\\FM150_SensorShield_RA6M3\\ra\\fsp\\inc\\api" -I"W:\\Workspace_2025-12\\FM150_SensorShield_RA6M3\\ra\\fsp\\inc\\instances" -I"W:\\Workspace_2025-12\\FM150_SensorShield_RA6M3\\ra\\arm\\CMSIS_6\\CMSIS\\Core\\Include" -I"W:\\Workspace_2025-12\\FM150_SensorShield_RA6M3\\Modules\\Scheduler" -D_RENESAS_RA_ -D_RA_CORE=CM4 -D_RA_ORDINAL=1 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -x c "$<" -c -o "$@")
	@clang --target=arm-none-eabi @"$@.in"

