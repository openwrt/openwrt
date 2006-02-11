# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=mrd6
PKG_VERSION:=0.9.5
PKG_RELEASE:=1
PKG_MD5SUM:=24a08cf1407000d628a272b08a415dda

PKG_SOURCE_URL:=http://hng.av.it.pt/mrd6/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

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
		DESTDIR=$(PKG_INSTALL_DIR) \
		PREFIX=/usr/ \
		install
	install -d -m0755 $(PKG_INSTALL_DIR)/etc/init.d
	install -m0644 files/$(PKG_NAME).conf $(PKG_INSTALL_DIR)/etc
	install -m0755 files/$(PKG_NAME).init $(PKG_INSTALL_DIR)/etc/init.d/S60$(PKG_NAME)
	touch $@

$(IPKG_MRD6): 
	mkdir -p $(IDIR_MRD6)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/ $(IDIR_MRD6)/
	$(RSTRIP) $(IDIR_MRD6)/
	$(IPKG_BUILD) $(IDIR_MRD6) $(PACKAGE_DIR)
