include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARD:=ipq807x
BOARDNAME:=Qualcomm IPQ807x
FEATURES:=squashfs ramdisk fpu nand source-only
KERNELNAME:=Image dtbs
CPU_TYPE:=cortex-a53
SUBTARGETS:=generic

KERNEL_PATCHVER:=5.4

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
