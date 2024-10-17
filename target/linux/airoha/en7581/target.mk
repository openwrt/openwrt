ARCH:=aarch64
SUBTARGET:=en7581
BOARDNAME:=EN7581
CPU_TYPE:=cortex-a53
KERNELNAME:=Image dtbs
FEATURES+=pwm source-only

define Target/Description
	Build firmware images for Airoha en7581 ARM based boards.
endef

