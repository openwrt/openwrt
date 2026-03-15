ARCH:=arm
BOARDNAME:=NXP i.MX with Cortex-A7
CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=neon-vfpv4
KERNELNAME:=zImage dtbs

define Target/Description
	Build firmware images for NXP i.MX (Cortex-A7) based boards.
endef
