# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=reaim
PKG_VERSION:=0.8
PKG_RELEASE:=1
PKG_MD5SUM:=647d2ab72ec454f89294accfb3182c09

PKG_SOURCE_URL:=@SF/reaim
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,REAIM,reaim,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		CFLAGS="$(TARGET_CFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_REAIM):
	install -d -m0755 $(IDIR_REAIM)/etc/init.d
	install -m0755 ./files/reaim.init $(IDIR_REAIM)/etc/init.d/reaim
	ln -sf reaim $(IDIR_REAIM)/etc/init.d/S63reaim
	install -d -m0755 $(IDIR_REAIM)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/reaim $(IDIR_REAIM)/usr/sbin/
	$(RSTRIP) $(IDIR_REAIM)
	$(IPKG_BUILD) $(IDIR_REAIM) $(PACKAGE_DIR)

