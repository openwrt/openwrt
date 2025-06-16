#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=linux-firmware
PKG_VERSION:=20250613
PKG_RELEASE:=2

PKG_SOURCE_URL:=@KERNEL/linux/kernel/firmware
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_HASH:=d40743337ca8bf9d2587e220ab30dec6a07b52cc5cb4a4ff8d92be14391a7fde

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

SCAN_DEPS = *.mk

include $(INCLUDE_DIR)/package.mk

# https://gitlab.freedesktop.org/drm/amd/-/issues/4329
POST_RELEASE_URL:=https://gitlab.com/kernel-firmware/linux-firmware/-/raw/9096bad65cb92da2925fe9ad891cbd4961fa6cda/amdgpu/

RSTRIP:=:
STRIP:=:

define Package/firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=http://git.kernel.org/cgit/linux/kernel/git/firmware/linux-firmware.git
  TITLE:=$(1)
  DEPENDS:=$(2)
  LICENSE_FILES:=$(3)
  LICENSE:=$(4)
endef

define Download/dcn_3_1_5_dmcub
  FILE:=dcn_3_1_5_dmcub.bin
  URL:=$(POST_RELEASE_URL)
  HASH:=37410c1c733baf0a9eaa77f41dafd578
endef

define Download/dcn_4_0_1_dmcub
  FILE:=dcn_4_0_1_dmcub.bin
  URL:=$(POST_RELEASE_URL)
  HASH:=e5fa221467fd5e1571fb4d69013ae2b9
endef

$(eval $(call Download,dcn_3_1_5_dmcub))
$(eval $(call Download,dcn_4_0_1_dmcub))

define Build/Compile

endef

include $(wildcard ./*.mk)
#$(eval $(call BuildPackage,linux-firmware))
