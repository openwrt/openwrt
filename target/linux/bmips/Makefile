# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

ARCH:=mips
CPU_TYPE:=mips32
BOARD:=bmips
BOARDNAME:=Broadcom BMIPS
SUBTARGETS:=bcm6318 bcm6328 bcm6358 bcm6362 bcm6368 bcm63268
FEATURES:=gpio squashfs usb

KERNEL_PATCHVER:=6.6

define Target/Description
	Build firmware images for BCM33xx cable modem chips,
	BCM63xx DSL chips and BCM7xxx set-top box chips.
endef

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += ethtool kmod-gpio-button-hotplug

$(eval $(call BuildTarget))
