# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=pcmcia-cs
PKG_VERSION:=3.2.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=0d6d65be8896eff081aee996049afaa5

include $(INCLUDE_DIR)/package.mk

define Package/pcmcia-cs
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@LINUX_2_4 @PCMCIA_SUPPORT
  TITLE:=PCMCIA/Cardbus management utilities
  URL:=http://pcmcia-cs.sourceforge.net/
endef

$(STAMP_CONFIGURED): $(LINUX_DIR)/.config

define Build/Configure
	( cd $(PKG_BUILD_DIR); \
		./Configure \
			--noprompt \
			--kernel="$(LINUX_DIR)" \
			--target="$(PKG_INSTALL_DIR)" \
			--arch="$(ARCH)" \
			--ucc="$(TARGET_CC)" \
			--uflags="$(TARGET_CFLAGS)" \
			--srctree \
			--cardbus \
			--noapm \
			--nopnp \
			--notrust \
			--nox11 \
			--rcdir="/etc" \
			--sysv \
	)
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(call Build/Compile/Default, \
		CFLAGS="$(TARGET_CFLAGS)" \
		all install \
	)
endef

define Package/pcmcia-cs/install
	$(INSTALL_DIR) $(1)/etc
	$(CP) $(PKG_INSTALL_DIR)/etc/pcmcia $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/cardctl $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/cardmgr $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,pcmcia-cs))
