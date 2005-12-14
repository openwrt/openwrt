# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=vrrpd
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_MD5SUM:=6d5066ea1a6ced817376ca0f54765447

PKG_SOURCE_URL:=@SF/vrrpd
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,VRRPD,vrrpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		DBG_OPT="" \
		MAIN_OPT="$(TARGET_CFLAGS)" \
		vrrpd
	touch $@

$(IPKG_VRRPD):
	install -d -m0755 $(IDIR_VRRPD)/etc/default
	install -m0644 ./files/vrrpd.default $(IDIR_VRRPD)/etc/default/vrrpd
	install -d -m0755 $(IDIR_VRRPD)/etc/init.d
	install -m0755 ./files/vrrpd.init $(IDIR_VRRPD)/etc/init.d/vrrpd
	install -d -m0755 $(IDIR_VRRPD)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/vrrpd $(IDIR_VRRPD)/usr/sbin/
	$(RSTRIP) $(IDIR_VRRPD)
	$(IPKG_BUILD) $(IDIR_VRRPD) $(PACKAGE_DIR)
