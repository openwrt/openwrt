# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=wireless-tools
PKG_VERSION:=28.pre6
PKG_RELEASE:=1
PKG_MD5SUM:=3ad1da3b17dff963eba32f0b79401253

PKG_SOURCE_URL:=http://www.hpl.hp.com/personal/Jean_Tourrilhes/Linux
PKG_SOURCE:=wireless_tools.$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/wireless_tools.28
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,WIRELESS_TOOLS,wireless-tools,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS) -I." \
		all iwmulticall
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		PREFIX="$(PKG_INSTALL_DIR)" \
		INSTALL_DIR="$(PKG_INSTALL_DIR)/usr/sbin" \
		INSTALL_LIB="$(PKG_INSTALL_DIR)/usr/lib" \
		install install-iwmulticall
	touch $(PKG_BUILD_DIR)/.built

$(IPKG_WIRELESS_TOOLS):
	mkdir -p $(IDIR_WIRELESS_TOOLS)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libiw.so.28 $(IDIR_WIRELESS_TOOLS)/usr/lib/
	mkdir -p $(IDIR_WIRELESS_TOOLS)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/iw{config,getid,list,priv,spy} $(IDIR_WIRELESS_TOOLS)/usr/sbin/
	$(RSTRIP) $(IDIR_WIRELESS_TOOLS)
	$(IPKG_BUILD) $(IDIR_WIRELESS_TOOLS) $(PACKAGE_DIR)
