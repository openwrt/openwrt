ARCH:=arm
SUBTARGET:=an7563
BOARDNAME:=AN7563
CPU_TYPE:=cortex-a7
KERNELNAME:=Image dtbs
FEATURES+=source-only

define Target/Description
	Build firmware images for Airoha an7563 (ARMv8 Cortex-A53 running
	in AArch32 mode) based boards.
endef
