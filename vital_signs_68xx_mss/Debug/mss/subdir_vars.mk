################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CFG_SRCS += \
../mss/mss_mmw.cfg 

CMD_SRCS += \
../mss/mss_mmw_linker.cmd 

C_SRCS += \
../mss/cli.c \
../mss/mss_main.c 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_MISC_DIRS += \
./configPkg/ 

C_DEPS += \
./mss/cli.d \
./mss/mss_main.d 

GEN_OPTS += \
./configPkg/compiler.opt 

OBJS += \
./mss/cli.oer4f \
./mss/mss_main.oer4f 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

OBJS__QUOTED += \
"mss\cli.oer4f" \
"mss\mss_main.oer4f" 

C_DEPS__QUOTED += \
"mss\cli.d" \
"mss\mss_main.d" 

GEN_FILES__QUOTED += \
"configPkg\linker.cmd" \
"configPkg\compiler.opt" 

C_SRCS__QUOTED += \
"../mss/cli.c" \
"../mss/mss_main.c" 


