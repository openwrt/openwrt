#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=linux-firmware
PKG_SOURCE_VERSION:=bbe4917c054eb0a73e250c6363341e3bf6725839
PKG_VERSION:=2015-12-07-$(PKG_SOURCE_VERSION)
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)
PKG_SOURCE_URL:=git://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git
PKG_MIRROR_MD5SUM:=ca4d289ad9380471cae376fc7dd3660a

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

include $(INCLUDE_DIR)/package.mk

define Package/firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=http://git.kernel.org/cgit/linux/kernel/git/firmware/linux-firmware.git
  TITLE:=$(1)
endef

define Build/Compile

endef

Package/ibt-firmware = $(call Package/firmware-default,Intel bluetooth firmware)
define Package/ibt-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/intel
	$(CP) \
		$(PKG_BUILD_DIR)/intel/*.bseq \
		$(1)/lib/firmware/intel
endef
$(eval $(call BuildPackage,ibt-firmware))

Package/ar3k-firmware = $(call Package/firmware-default,ath3k firmware)
define Package/ar3k-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/ar3k
	$(CP) \
		$(PKG_BUILD_DIR)/ar3k/*.dfu \
		$(1)/lib/firmware/ar3k
endef
$(eval $(call BuildPackage,ar3k-firmware))

include $(wildcard ./*.mk)
#$(eval $(call BuildPackage,linux-firmware))
