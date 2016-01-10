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

define Package/ibt-firmware
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=http://git.kernel.org/cgit/linux/kernel/git/firmware/linux-firmware.git
  TITLE:=Intel bluetooth firmware
endef

define Package/ibt-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/intel
	$(CP) \
		$(PKG_BUILD_DIR)/intel/*.bseq \
		$(1)/lib/firmware/intel
endef

define Package/ar3k-firmware
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=http://git.kernel.org/cgit/linux/kernel/git/firmware/linux-firmware.git
  TITLE:=ath3k firmware
endef

define Package/ar3k-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/ar3k
	$(CP) \
		$(PKG_BUILD_DIR)/ar3k/*.dfu \
		$(1)/lib/firmware/ar3k
endef

define Build/Compile

endef

include $(wildcard ./*.mk)

$(eval $(call BuildPackage,ar3k-firmware))
$(eval $(call BuildPackage,ibt-firmware))
