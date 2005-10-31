# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=wrt-radauth
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_MD5SUM:=f941e3139175dc6b87f6c89ba8848a07

PKG_SOURCE_URL:=http://www.zerfleddert.de/wrt54g/
PKG_SOURCE:=$(PKG_NAME).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

BR2_PACKAGE_WRT_RADAUTH:=m

$(eval $(call PKG_template,WRT_RADAUTH,wrt-radauth,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		clean all
	touch $@

$(IPKG_WRT_RADAUTH):
	install -d -m0755 $(IDIR_WRT_RADAUTH)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/radius-client $(IDIR_WRT_RADAUTH)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/wrt-radauth $(IDIR_WRT_RADAUTH)/usr/sbin/
	$(RSTRIP) $(IDIR_WRT_RADAUTH)
	$(IPKG_BUILD) $(IDIR_WRT_RADAUTH) $(PACKAGE_DIR)
