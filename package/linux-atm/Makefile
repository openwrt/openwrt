# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=linux-atm
PKG_VERSION:=2.4.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/l/linux-atm/
PKG_MD5SUM:=84fef49cc39ff2605204246666f65864

include $(INCLUDE_DIR)/package.mk

define Package/linux-atm
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Linux ATM library
  URL:=http://linux-atm.sourceforge.net/
endef

define Package/linux-atm/description
  This package contains a library for accessing the Linux ATM subsystem.
endef

define Package/atm-tools
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=linux-atm
  TITLE:=Linux ATM tools
  URL:=http://linux-atm.sourceforge.net/
endef

define Package/atm-tools/description
  This package contains the Linux ATM tools.
endef

define Build/Configure
	$(call Build/Configure/Default)
	# prevent autoheader invocation
	touch $(PKG_BUILD_DIR)/stamp-h.in
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP)	$(PKG_INSTALL_DIR)/usr/include/atm{,d,sap}.h \
		$(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP)	$(PKG_INSTALL_DIR)/usr/lib/libatm.{a,so*} \
		$(STAGING_DIR)/usr/lib/
endef

define Build/UninstallDev
	rm -rf	$(STAGING_DIR)/usr/include/atm{,d,sap}.h \
		$(STAGING_DIR)/usr/lib/libatm.{a,so*}
endef

define Package/linux-atm/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libatm.so.* $(1)/usr/lib/
endef

define Package/atm-tools/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/atmarp{,d} $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_BIN) ./files/ipoa.sh $(1)/lib/network/
endef

$(eval $(call BuildPackage,linux-atm))
$(eval $(call BuildPackage,atm-tools))
