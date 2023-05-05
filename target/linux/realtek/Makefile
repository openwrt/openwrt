# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=realtek
BOARDNAME:=Realtek based boards
FEATURES:=ramdisk squashfs
SUBTARGETS:=rtl838x rtl839x rtl930x rtl931x

KERNEL_PATCHVER:=5.15

define Target/Description
	Build firmware images for Realtek switch based boards.
endef

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += ethtool

$(eval $(call BuildTarget))
