# $Id: Makefile 1146 2005-06-05 13:32:28Z florian $

include $(TOPDIR)/rules.mk

PKG_NAME:=mrd6
PKG_VERSION:=0.9.4-beta2
PKG_RELEASE:=1
PKG_MD5SUM:=fe2e617954eccacd9b5c3d6e85f41c89

PKG_SOURCE_URL:=http://hng.av.it.pt/mrd6/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MRD6,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)/src \
		OPTIMIZE=yes \
		SPACE_OPTIMIZE=yes \
		FULL_STATIC=yes \
		CFLAGS="$(TARGET_CFLAGS) -DNO_INET6_OPTION" \
		CXX=$(TARGET_CROSS)g++ \
		CC=$(TARGET_CC) \
		STAGING_DIR=$(STAGING_DIR) \
		PREFIX=/usr \
		DESTDIR=$(PKG_INSTALL_DIR) \
		install
	install -d -m0755 $(PKG_INSTALL_DIR)/etc/init.d
	install -m0644 files/$(PKG_NAME).conf $(PKG_INSTALL_DIR)/etc
	install -m0755 files/$(PKG_NAME).init $(PKG_INSTALL_DIR)/etc/init.d/S60$(PKG_NAME)
	touch $@

$(IPKG_MRD6): 
	mkdir -p $(IDIR_MRD6)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/ $(IDIR_MRD6)/
	$(RSTRIP) $(IDIR_MRD6)/
	$(IPKG_BUILD) $(IDIR_MRD6) $(PACKAGE_DIR)
