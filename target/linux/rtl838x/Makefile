# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
CPU_TYPE:=4kec
BOARD:=rtl838x
BOARDNAME:=Realtek MIPS
DEVICE_TYPE:=basic
FEATURES:=ramdisk squashfs

KERNEL_PATCHVER:=5.4

define Target/Description
	Build firmware images for Realtek RTL838x based boards.
endef

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += swconfig uboot-envtools ethtool kmod-gpio-button-hotplug

$(eval $(call BuildTarget))
