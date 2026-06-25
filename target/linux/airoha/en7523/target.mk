ARCH:=arm
SUBTARGET:=en7523
BOARDNAME:=EN7523 / EN7529 / EN7562
CPU_TYPE:=cortex-a7
KERNELNAME:=Image dtbs vmlinuz
FEATURES+=source-only

DEFAULT_PACKAGES += \
	airoha-en7523-npu-firmware

define Target/Description
  Build firmware images for Airoha en7523 ARM based boards.
endef
