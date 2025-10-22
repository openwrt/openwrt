# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=realtek
BOARDNAME:=Realtek MIPS
DEVICE_TYPE:=basic
FEATURES:=ramdisk squashfs
SUBTARGETS:=rtl838x rtl839x rtl930x rtl930x_nand rtl931x rtl931x_nand

KERNEL_PATCHVER:=6.12

define Target/Description
	Build firmware images for Realtek RTL83xx/RTL93xx based boards.
endef

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	ethtool \
	firewall4 \
	kmod-gpio-button-hotplug \
	odhcp6c \
	uboot-envtools \


$(eval $(call BuildTarget))
