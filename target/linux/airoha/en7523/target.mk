ARCH:=arm
SUBTARGET:=en7523
BOARDNAME:=EN7523 / EN7529 / EN7562
CPU_TYPE:=cortex-a7
KERNELNAME:=Image dtbs
FEATURES+=source-only

define Target/Description
  Build firmware images for Airoha an7523 ARM based boards.
endef
