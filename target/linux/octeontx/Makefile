# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2018 OpenWrt.org

include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARD:=octeontx
BOARDNAME:=Octeon-TX
FEATURES:=squashfs ramdisk targz pcie gpio rtc usb fpu
SUBTARGETS:=generic

KERNEL_PATCHVER:=5.15

define Target/Description
	Build images for Octeon-TX CN80XX/CN81XX based boards
endef

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=Image

DEFAULT_PACKAGES += uboot-envtools mkf2fs e2fsprogs blkid \
		    kmod-hwmon-gsc kmod-leds-gpio kmod-pps-gpio \
		    kmod-gpio-button-hotplug kmod-input-evdev kmod-rtc-ds1672 \
		    kmod-can kmod-can-mcp251x

$(eval $(call BuildTarget))
