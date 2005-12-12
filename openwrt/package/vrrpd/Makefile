include $(TOPDIR)/rules.mk

PKG_NAME:=vrrpd
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_MD5SUM:=6d5066ea1a6ced817376ca0f54765447
PKG_SOURCE_URL:=@SF/vrrpd

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,VRRPD,VRRPD,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	mkdir -p $(PKG_INSTALL_DIR)/usr/sbin
	$(MAKE) CC=$(TARGET_CC) -C $(PKG_BUILD_DIR)
	install -m0755 $(PKG_BUILD_DIR)/vrrpd $(PKG_INSTALL_DIR)/usr/sbin/
	touch $@

$(IPKG_VRRPD):
	mkdir -p $(IDIR_VRRPD)
	cp -fpR $(PKG_INSTALL_DIR)/* $(IDIR_VRRPD)/
	$(RSTRIP) $(IDIR_VRRPD)
	$(IPKG_BUILD) $(IDIR_VRRPD) $(PACKAGE_DIR)
