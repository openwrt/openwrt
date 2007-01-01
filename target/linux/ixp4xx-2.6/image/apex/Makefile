# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=apex
PKG_VERSION:=1.4.7
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://metalab.unc.edu/pub/Linux/system/boot/apex/
PKG_MD5SUM:=ed2183311aacf5714afbd825f3671417
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(TARGET)

include $(INCLUDE_DIR)/package.mk

define Package/apex-nslu2
  TITLE:=apex-nslu2
  DESCRIPTION:=\
	The Apex Bootloader (compiled for the Linksys NSLU2).
  URL:=http://wiki.buici.com/twiki/bin/view/Main/ApexBootloader
  SECTION:=base
  CATEGORY:=Boot Loaders
  DEPENDS:=@LINUX_2_6_IXP4XX
endef

define Build/Configure
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH=arm \
		slugos-nslu2-armeb_config
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		ARCH=arm \
		all
endef

compile-targets: ${PKG_BUILD_DIR}/.built

ifneq ($(TARGET),)
install: compile
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/apex.bin $(TARGET)/apex-nslu2-armeb.bin
endif

$(eval $(call BuildPackage,apex-nslu2))
