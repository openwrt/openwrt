# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=wireless-tools
PKG_VERSION:=28
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/wireless_tools.28
PKG_SOURCE:=wireless_tools.$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.hpl.hp.com/personal/Jean_Tourrilhes/Linux
PKG_MD5SUM:=599c94497f9c9073c7b052d3dcb7cd16
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/include/package.mk

define Package/wireless-tools
SECTION:=base
CATEGORY:=Network
DEFAULT:=y
TITLE:=Tools for manipulating Linux Wireless Extensions
DESCRIPTION:=Tools for configuring Wireless Adapters implementing the "Wireless Extension"
URL:=http://hplabs.hp.com/personal/Jean_Tourrilhes/Linux/Tools.html
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS) -I." \
		libiw.so.28 iwmulticall
	$(MAKE) -C $(PKG_BUILD_DIR) \
		PREFIX="$(PKG_INSTALL_DIR)" \
		INSTALL_DIR="$(PKG_INSTALL_DIR)/usr/sbin" \
		INSTALL_LIB="$(PKG_INSTALL_DIR)/usr/lib" \
		install-iwmulticall
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_BUILD_DIR)/{iwlib,wireless}.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libiw.so* $(STAGING_DIR)/usr/lib/
	ln -sf libiw.so.28 $(STAGING_DIR)/usr/lib/libiw.so
endef
	
define Build/UninstallDev
	rm -f	$(STAGING_DIR)/usr/include/{iwlib,wireless}.h \
		$(STAGING_DIR)/usr/lib/libiw.so*
endef

define Package/wireless-tools/install
	install -d -m0755  $(1)/usr/lib
	install -m0755 $(PKG_BUILD_DIR)/libiw.so.28 $(1)/usr/lib/
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_INSTALL_DIR)/usr/sbin/iw{config,getid,list,priv,spy} $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,wireless-tools))
