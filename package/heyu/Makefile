# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=heyu
PKG_VERSION:=2.0beta.5
PKG_RELEASE:=1
PKG_MD5SUM:=8289d5ca5b972333a72bc0fc3e640c13
PKG_SOURCE_URL:=http://heyu.tanj.com/download/
PKG_SOURCE:=heyu-$(PKG_VERSION).tgz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,HEYU,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: 
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld \
		CFLAGS="$(TARGET_CFLAGS) -I$(PKG_BUILD_DIR) -DSYSV -DPOSIX -DHAS_ITIMER -DLINUX -DHASSELECT -DHASTZ"
	touch $@

$(IPKG_HEYU):
	install -d -m0755 $(IDIR_HEYU)/etc/$(PKG_NAME)
	install -m0644 ./files/x10.conf $(IDIR_HEYU)/etc/$(PKG_NAME)/x10.conf
	install -d -m0755 $(IDIR_HEYU)/usr/bin
	install -m0644 $(PKG_BUILD_DIR)/heyu $(IDIR_HEYU)/usr/bin/
	$(RSTRIP) $(IDIR_HEYU)
	$(IPKG_BUILD) $(IDIR_HEYU) $(PACKAGE_DIR)
