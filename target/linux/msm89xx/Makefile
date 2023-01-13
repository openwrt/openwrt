# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

BOARD:=msm89xx
BOARDNAME:=Boards based on Snapdragon 210/410/615 chipsets
FEATURES:=ext4 usb usbgadget gpio fpu rootfs-part boot-part squashfs separate_ramdisk
SUBTARGETS:=msm8916

KERNEL_PATCHVER:=6.1

define Target/Description
	Build firmware images for various Snapdragon 210/410/615 based devices
endef

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += partx-utils e2fsprogs mkf2fs
DEFAULT_PACKAGES += kmod-usb-chipidea kmod-usb-gadget-eth
DEFAULT_PACKAGES += kmod-wcn36xx kmod-rproc-wcnss
DEFAULT_PACKAGES += kmod-rpmsg-wwan-ctrl kmod-bam-dmux kmod-qcom-rproc-modem

KERNELNAME:=Image

$(eval $(call BuildTarget))
