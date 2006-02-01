include $(TOPDIR)/rules.mk

PKG_NAME:=p910nd
PKG_VERSION:=0.7
PKG_RELEASE:=2
PKG_MD5SUM:=7bf752532d26c9106f8039db95df3a6b

PKG_SOURCE_URL:=http://www.etherboot.org/p910nd
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

PKG_INIT_PRIO=70

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,P910ND,p910nd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -DLOCKFILE_DIR=\"\\\"/tmp\\\"\""
	touch $@

$(IPKG_P910ND):
	install -d -m0755 $(IDIR_P910ND)/etc/default
	install -m0644 ./files/p910nd.default $(IDIR_P910ND)/etc/default/p910nd
	install -d -m0755 $(IDIR_P910ND)/etc/init.d
	install -m0755 ./files/p910nd.init $(IDIR_P910ND)/etc/init.d/p910nd
	install -d -m0755 $(IDIR_P910ND)/usr/sbin
	ln -sf p910nd $(IDIR_P910ND)/etc/init.d/S$(PKG_INIT_PRIO)p910nd
	$(CP) $(PKG_BUILD_DIR)/p910nd $(IDIR_P910ND)/usr/sbin
	$(RSTRIP) $(IDIR_P910ND)
	$(IPKG_BUILD) $(IDIR_P910ND) $(PACKAGE_DIR)

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -rf $(PKG_BUILD_DIR)/.built
