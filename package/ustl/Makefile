# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ustl
PKG_VERSION:=0.8
PKG_RELEASE:=1
PKG_MD5SUM:=dd59483c327f1bd1356273bc5ae4dde0

PKG_SOURCE_URL:=@SF/ustl
PKG_SOURCE:=$(PKG_NAME)-0.8-0.tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

BR2_PACKAGE_USTL:=m

$(eval $(call PKG_template,USTL,ustl,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	$(CP) ./files/config.h $(PKG_BUILD_DIR)/
	$(CP) ./files/Common.mk $(PKG_BUILD_DIR)/
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CROSS="$(TARGET_CROSS)" \
		OPTFLAGS="$(TARGET_CFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_USTL):
	install -d -m0755 $(IDIR_USTL)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libustl.so.* $(IDIR_USTL)/usr/lib/
	$(RSTRIP) $(IDIR_USTL)
	$(IPKG_BUILD) $(IDIR_USTL) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libustl.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/ustl* $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libustl.a $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libustl.so* $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libustl.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/ustl* \
		$(STAGING_DIR)/usr/lib/libustl.a \
		$(STAGING_DIR)/usr/lib/libustl.so* \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
