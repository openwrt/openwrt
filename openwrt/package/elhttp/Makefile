# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=elhttp
PKG_VERSION:=0.22
PKG_RELEASE:=1
PKG_MD5SUM:=b52f68c47f709e9fc9429250eb8f954c

PKG_SOURCE_URL:=http://phadej.daug.net/files/ \
	http://www.cr0.net:8040/code/network/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,ELHTTP,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	mkdir -p $(PKG_INSTALL_DIR)/usr/sbin
	$(MAKE) CC=$(TARGET_CC) CROSS=$(TARGET_CROSS) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	$(RSTRIP) $(PKG_INSTALL_DIR)/usr/sbin/
	touch $@

$(IPKG_ELHTTP):
	mkdir -p $(IDIR_ELHTTP)
	$(CP) $(PKG_INSTALL_DIR)/* $(IDIR_ELHTTP)/ 
	$(RSTRIP) $(IDIR_ELHTTP)/
	$(IPKG_BUILD) $(IDIR_ELHTTP) $(PACKAGE_DIR)

