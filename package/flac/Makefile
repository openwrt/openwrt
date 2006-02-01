# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=flac
PKG_VERSION:=1.1.2
PKG_RELEASE:=1
PKG_MD5SUM:=2bfc127cdda02834d0491ab531a20960

PKG_SOURCE_URL:=@SF/flac
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBFLAC,libflac,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) -f Makefile.lite \
		$(TARGET_CONFIGURE_OPTS) \
		RELEASE_CFLAGS="$(TARGET_CFLAGS)" \
		libFLAC
	touch $@

$(IPKG_LIBFLAC):
	install -m0755 -d $(IDIR_LIBFLAC)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/obj/release/lib/libFLAC.so $(IDIR_LIBFLAC)/usr/lib/
	$(RSTRIP) $(IDIR_LIBFLAC)
	$(IPKG_BUILD) $(IDIR_LIBFLAC) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libFLAC.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_BUILD_DIR)/include/FLAC $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/obj/release/lib/libFLAC.a $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/obj/release/lib/libFLAC.so $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libFLAC.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/FLAC \
		$(STAGING_DIR)/usr/lib/libFLAC.a \
		$(STAGING_DIR)/usr/lib/libFLAC.so \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built

