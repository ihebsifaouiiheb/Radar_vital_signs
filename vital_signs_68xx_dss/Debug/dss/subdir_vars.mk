################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CFG_SRCS += \
../dss/dss_mmw.cfg 

CMD_SRCS += \
../dss/c674x_linker.cmd \
../dss/dss_mmw_linker.cmd 

C_SRCS += \
../dss/dss_config_edma_util.c \
../dss/dss_data_path.c \
../dss/dss_main.c \
../dss/dss_vitalSignsDemo_utilsFunc.c 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_MISC_DIRS += \
./configPkg/ 

C_DEPS += \
./dss/dss_config_edma_util.d \
./dss/dss_data_path.d \
./dss/dss_main.d \
./dss/dss_vitalSignsDemo_utilsFunc.d 

GEN_OPTS += \
./configPkg/compiler.opt 

OBJS += \
./dss/dss_config_edma_util.oe674 \
./dss/dss_data_path.oe674 \
./dss/dss_main.oe674 \
./dss/dss_vitalSignsDemo_utilsFunc.oe674 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

OBJS__QUOTED += \
"dss\dss_config_edma_util.oe674" \
"dss\dss_data_path.oe674" \
"dss\dss_main.oe674" \
"dss\dss_vitalSignsDemo_utilsFunc.oe674" 

C_DEPS__QUOTED += \
"dss\dss_config_edma_util.d" \
"dss\dss_data_path.d" \
"dss\dss_main.d" \
"dss\dss_vitalSignsDemo_utilsFunc.d" 

GEN_FILES__QUOTED += \
"configPkg\linker.cmd" \
"configPkg\compiler.opt" 

C_SRCS__QUOTED += \
"../dss/dss_config_edma_util.c" \
"../dss/dss_data_path.c" \
"../dss/dss_main.c" \
"../dss/dss_vitalSignsDemo_utilsFunc.c" 


