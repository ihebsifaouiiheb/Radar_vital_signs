################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ti-cgt-c6000_8.3.3

GEN_OPTS__FLAG := --cmd_file="configPkg/compiler.opt" 
GEN_CMDS__FLAG := -l"configPkg/linker.cmd" 

ORDERED_OBJS += \
"./common/mmwDemo_monitor.oe674" \
"./dss/dss_config_edma_util.oe674" \
"./dss/dss_data_path.oe674" \
"./dss/dss_main.oe674" \
"./dss/dss_vitalSignsDemo_utilsFunc.oe674" \
"../dss/c674x_linker.cmd" \
"../dss/dss_mmw_linker.cmd" \
$(GEN_CMDS__FLAG) \
-ldsplib.ae64P \
-lmathlib.ae674 \
-llibsoc_xwr68xx.ae674 \
-llibosal_xwr68xx.ae674 \
-llibcrc_xwr68xx.ae674 \
-llibmailbox_xwr68xx.ae674 \
-llibmmwavelink_xwr68xx.ae674 \
-llibmmwave_xwr68xx.ae674 \
-llibedma_xwr68xx.ae674 \
-llibadcbuf_xwr68xx.ae674 \
-llibmmwavealg_xwr68xx.ae674 \
-llibcbuff_xwr68xx.ae674 \
-llibhsiheader_xwr68xx.ae674 \
-lrts6740_elf.lib \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include common/subdir_vars.mk
-include dss/subdir_vars.mk
-include common/subdir_rules.mk
-include dss/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
vital_signs_68xx_dss.xe674 \

EXE_OUTPUTS__QUOTED += \
"vital_signs_68xx_dss.xe674" \

BIN_OUTPUTS += \
vital_signs_68xx_dss.hex \

BIN_OUTPUTS__QUOTED += \
"vital_signs_68xx_dss.hex" \


# All Target
all:
	@$(MAKE) --no-print-directory pre-build
	@$(MAKE) --no-print-directory main-build

# Main-build Target
main-build: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "vital_signs_68xx_dss.xe674"

# Tool invocations
vital_signs_68xx_dss.xe674: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C6000 Linker'
	"C:/ti/ti-cgt-c6000_8.3.3/bin/cl6x" -mv6740 --abi=eabi -O3 -ms0 --define=SOC_XWR68XX --define=SUBSYS_DSS --define=MMWAVE_L3RAM_NUM_BANK=6 --define=MMWAVE_SHMEM_TCMA_NUM_BANK=0 --define=MMWAVE_SHMEM_TCMB_NUM_BANK=0 --define=MMWAVE_SHMEM_BANK_SIZE=0x20000 --define=DOWNLOAD_FROM_CCS --define=DebugP_ASSERT_ENABLED --define=_LITTLE_ENDIAN -g --gcc --diag_error=10015 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --obj_extension=.oe674 -z -m"vital_signs_68xx_dss.map" --heap_size=0x800 --stack_size=0x800 -i"C:/ti/ti-cgt-c6000_8.3.3/lib" -i"C:/Users/ISIFAOUI/Desktop/WorkSpace/CCS/fasa5/vital_signs_68xx_dss" -i"C:/ti/mmwave_sdk_03_05_00_04/packages/ti/control/mmwave/lib" -i"C:/ti/mmwave_sdk_03_05_00_04/packages/ti/control/mmwavelink/lib" -i"C:/ti/mmwave_sdk_03_05_00_04/packages/ti/alg/mmwavelib/lib" -i"C:/ti/mmwave_sdk_03_05_00_04/packages/ti/drivers/crc/lib" -i"C:/ti/mmwave_sdk_03_05_00_04/packages/ti/drivers/soc/lib" -i"C:/ti/mmwave_sdk_03_05_00_04/packages/ti/drivers/osal/lib" -i"C:/ti/mmwave_sdk_03_05_00_04/packages/ti/drivers/mailbox/lib" -i"C:/ti/mmwave_sdk_03_05_00_04/packages/ti/drivers/edma/lib" -i"C:/ti/mmwave_sdk_03_05_00_04/packages/ti/drivers/adcbuf/lib" -i"C:/ti/mmwave_sdk_03_05_00_04/packages/ti/drivers/cbuff/lib" -i"C:/ti/mmwave_sdk_03_05_00_04/packages/ti/utils/hsiheader/lib" -i"C:/ti/dsplib_c64Px_3_4_0_0/packages/ti/dsplib/lib" -i"C:/ti/mathlib_c674x_3_1_2_1/packages/ti/mathlib/lib" -i"C:/ti/ti-cgt-c6000_8.3.3/include" --reread_libs --disable_auto_rts --define=MMWAVE_L3RAM_NUM_BANK=6 --define=MMWAVE_SHMEM_TCMA_NUM_BANK=0 --define=MMWAVE_SHMEM_TCMB_NUM_BANK=0 --define=MMWAVE_SHMEM_BANK_SIZE=0x20000 --diag_error=10015 --diag_warning=225 --diag_wrap=off --display_error_number --warn_sections --xml_link_info="vital_signs_68xx_dss_linkInfo.xml" --ram_model --unused_section_elimination=on -o "vital_signs_68xx_dss.xe674" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

vital_signs_68xx_dss.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: C6000 Hex Utility'
	"C:/ti/ti-cgt-c6000_8.3.3/bin/hex6x" --diag_wrap=off -o "vital_signs_68xx_dss.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "common\mmwDemo_monitor.oe674" "dss\dss_config_edma_util.oe674" "dss\dss_data_path.oe674" "dss\dss_main.oe674" "dss\dss_vitalSignsDemo_utilsFunc.oe674" 
	-$(RM) "common\mmwDemo_monitor.d" "dss\dss_config_edma_util.d" "dss\dss_data_path.d" "dss\dss_main.d" "dss\dss_vitalSignsDemo_utilsFunc.d" 
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-@echo 'Finished clean'
	-@echo ' '

pre-build:
	-rm -f "C:/Users/ISIFAOUI/Desktop/WorkSpace/CCS/fasa5/vital_signs_68xx_dss/Debug/vital_signs_68xx_dss.bin"
	-@echo ' '

post-build:
	-C:/ti/mmwave_sdk_03_05_00_04/packages/scripts/ImageCreator/out2rprc/out2rprc.exe vital_signs_68xx_dss.xe674 vital_signs_68xx_dss.bin
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

