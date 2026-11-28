ARCH:=arm
SUBTARGET:=ipq50xx32
BOARDNAME:=Qualcomm Atheros IPQ50xx (32-bit)
CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=neon-vfpv4
FEATURES:=fpu ramdisk source-only squashfs

define Target/Description
	Build a 32-bit kernel and packages for Qualcomm Atheros IPQ50xx.
endef
