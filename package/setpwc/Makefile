# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=setpwc
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_MD5SUM:=a125d76f630c4aab940df5912f161965

PKG_SOURCE_URL:=http://www.vanheusden.com/setpwc/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,SETPWC,setpwc,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(TARGET_CC) $(TARGET_CFLAGS) -DVERSION=\"$(PKG_VERSION)\" -o $(PKG_BUILD_DIR)/setpwc $(PKG_BUILD_DIR)/setpwc.c 
	touch $@

$(IPKG_SETPWC):
	install -d -m0755 $(IDIR_SETPWC)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/setpwc $(IDIR_SETPWC)/usr/bin/
	$(RSTRIP) $(IDIR_SETPWC)
	$(IPKG_BUILD) $(IDIR_SETPWC) $(PACKAGE_DIR)
