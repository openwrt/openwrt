# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

BOARD:=realtek
BOARDNAME:=Realtek MIPS/ARM based Router/Media SoC
FEATURES:=ramdisk squashfs
SUBTARGETS:=rtl838x rtl839x rtl930x rtl931x rtd129x rtd1319 rtd1619b

KERNEL_PATCHVER:=5.15

define Target/Description
	Build firmware images for Realtek Router/NAS boards.
endef

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	ethtool \
	firewall4 \
	ip-bridge \
	ip-full \
	kmod-gpio-button-hotplug \
	kmod-nft-offload \
	nftables \
	odhcp6c \
	tc-bpf \
	uboot-envtools \


$(eval $(call BuildTarget))
