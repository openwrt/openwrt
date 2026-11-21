# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=dspg
BOARDNAME:=DSP Group DVF99
FEATURES:=ramdisk squashfs gpio nand usb
CPU_TYPE:=arm926ej-s
SUBTARGETS:=dvf99

KERNEL_PATCHVER:=6.18

KERNELNAME:=zImage

define Target/Description
	Build firmware images for DSP Group DVF99 based devices.
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))
