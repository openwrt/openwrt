# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=shat
PKG_VERSION:=0.9.7
PKG_RELEASE:=1
PKG_MD5SUM:= da06846f35838b14d1513523dad84d24

PKG_SOURCE_URL:=http://world.maxina.de/src/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,SHAT,shat,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)/src \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld \
		CFLAGS="$(TARGET_CFLAGS) -Wall -I$(PKG_BUILD_DIR)/include/linux/include -I$(PKG_BUILD_DIR)/include -DVERSION=\\\"$(PKG_VERSION)\\\" -DDROP_NETBIOS"
	touch $@

$(IPKG_SHAT):
	install -d -m0755 $(IDIR_SHAT)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/src/shatd $(IDIR_SHAT)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/src/shatc $(IDIR_SHAT)/usr/sbin/
	$(RSTRIP) $(IDIR_SHAT)
	$(IPKG_BUILD) $(IDIR_SHAT) $(PACKAGE_DIR)
