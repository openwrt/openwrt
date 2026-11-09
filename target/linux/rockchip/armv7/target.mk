ARCH:=arm
SUBTARGET:=armv7
BOARDNAME:=RV11xx boards (32 bit)
CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=neon-vfpv4
FEATURES+=emmc
KERNELNAME:=zImage dtbs

define Target/Description
	Build firmware image for Rockchip RV11xx devices.
	This firmware features a 32 bit kernel.
endef
