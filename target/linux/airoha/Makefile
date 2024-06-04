include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=airoha
BOARDNAME:=Airoha ARM
CPU_TYPE:=cortex-a7
FEATURES:=dt squashfs nand ramdisk gpio source-only

KERNEL_PATCHVER:=6.6

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=Image dtbs

$(eval $(call BuildTarget))
