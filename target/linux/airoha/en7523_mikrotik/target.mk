ARCH:=arm
SUBTARGET:=en7523
BOARDNAME:=EN7523 / EN7529 / EN7562 for Mikrotik
CPU_TYPE:=cortex-a7
KERNELNAME:=zImage Image dtbs
FEATURES+=source-only
KERNEL_IMAGES:=vmlinux
IMAGES_DIR:=compressed

define Target/Description
  Build firmware images for Airoha an7523 ARM Mikrotik devices.
endef
