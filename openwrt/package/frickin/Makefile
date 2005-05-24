# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=frickin
PKG_VERSION:=1.2
PKG_RELEASE:=1
PKG_MD5SUM:= dd703f9202b3c0e55237d04ea429c684

PKG_SOURCE_URL:=@SF/frickin
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,FRICKIN,frickin,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		all
	touch $@

$(IPKG_FRICKIN):
	install -d -m0755 $(IDIR_FRICKIN)/etc/default
	install -m0644 ./files/frickin.default $(IDIR_FRICKIN)/etc/default/frickin
	install -d -m0755 $(IDIR_FRICKIN)/etc/init.d
	install -m0755 ./files/frickin.init $(IDIR_FRICKIN)/etc/init.d/frickin
	install -d -m0755 $(IDIR_FRICKIN)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/frickin $(IDIR_FRICKIN)/usr/sbin/
	$(RSTRIP) $(IDIR_FRICKIN)
	$(IPKG_BUILD) $(IDIR_FRICKIN) $(PACKAGE_DIR)
