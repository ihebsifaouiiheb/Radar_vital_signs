# invoke SourceDir generated makefile for mss_mmw.per4ft
mss_mmw.per4ft: .libraries,mss_mmw.per4ft
.libraries,mss_mmw.per4ft: package/cfg/mss_mmw_per4ft.xdl
	$(MAKE) -f C:\Users\ISIFAOUI\Desktop\WorkSpace\CCS\ccs\vital_signs_68xx_mss\mss/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\ISIFAOUI\Desktop\WorkSpace\CCS\ccs\vital_signs_68xx_mss\mss/src/makefile.libs clean

