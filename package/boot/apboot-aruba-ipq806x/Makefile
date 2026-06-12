#
# Copyright (C) 2026 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=apboot-aruba-ipq806x
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/lukasstockner/ap325-apboot-openwrt.git
PKG_MIRROR_HASH:=ef91bc1539d50c19c35ae5d38eab1a4944692d3f651952f58800ea1f7ffd4878
PKG_SOURCE_DATE:=2026-06-12
PKG_SOURCE_VERSION:=d36bb7270a8a8a5c5988ff778e55de12e0c40745

PKG_LICENSE:=GPL-2.0-or-later
PKG_LICENSE_FILES:=COPYING
PKG_MAINTAINER:=Lukas Stockner <lukas@lukasstockner.de>

# Legacy U-Boot tree, not Kbuild-parallel safe.
PKG_BUILD_PARALLEL:=0
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

# Aruba/QCA board name in the APBoot tree (Makefile.releng target):
#   octomore -> AP-324 / AP-325 (and Siemens Scalance W1750D)
APBOOT_BOARD:=octomore

# Stamped into the version banner; the upstream Dockerfile uses the short hash.
APBOOT_LABELID:=$(call version_abbrev,$(PKG_SOURCE_VERSION))

define Package/apboot-aruba-ipq806x
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_ipq806x
  TITLE:=Patched Aruba APBoot for AP-32x
  URL:=https://github.com/lukasstockner/ap325-apboot-openwrt
endef

define Package/apboot-aruba-ipq806x/description
  Patched build of the Aruba APBoot bootloader (a vendor fork of U-Boot) for
  the Aruba AP-324 / AP-325 access points (board "octomore"). The factory
  bootloader enforces RSA signatures on the firmware; this build removes the
  signature check and switches the console to 115200 baud so that OpenWrt can
  be booted. The resulting "u-boot.mbn" is meant to be flashed to the APPSBL
  SPI-flash partition via the serial console.
endef

# APBoot is a freestanding U-Boot, so build it with OpenWrt's target
# cross-toolchain instead of the bare-metal arm-none-eabi- default. The legacy
# Makefile honours an externally supplied CROSS_COMPILE.
define Build/Compile
	+$(MAKE) -C $(PKG_BUILD_DIR) -f Makefile.releng $(APBOOT_BOARD) \
		SHELL=/bin/bash \
		CROSS_COMPILE=$(TARGET_CROSS) \
		HOSTCC="$(HOSTCC)" \
		LABELID=$(APBOOT_LABELID) \
		ARUBA_MAKE_VERBOSE=1
endef

# The bootloader is flashed by hand and is not part of the root filesystem.
# Stage the image so the ipq806x image build can emit it as a device artifact.
define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/u-boot.mbn \
		$(STAGING_DIR_IMAGE)/aruba_ap-32x-apboot.mbn
endef

define Package/apboot-aruba-ipq806x/install
	:
endef

$(eval $(call BuildPackage,apboot-aruba-ipq806x))
