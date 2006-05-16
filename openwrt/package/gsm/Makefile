# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=gsm
PKG_VERSION:=1.0.10
PKG_RELEASE:=1
PKG_MD5SUM:=4b148480f82e96d274248e13880ec873

PKG_SOURCE_URL:=ftp://ftp.cs.tu-berlin.de/pub/local/kbs/tubmik/gsm/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-1.0-pl10
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBGSM,libgsm,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,GSM_UTILS,gsm-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		LD="$(TARGET_CC)" \
		COPTS="$(TARGET_CFLAGS)" \
		INSTALL_ROOT="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBGSM):
	install -d -m0755 $(IDIR_LIBGSM)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgsm.so.* $(IDIR_LIBGSM)/usr/lib/
	$(RSTRIP) $(IDIR_LIBGSM)
	$(IPKG_BUILD) $(IDIR_LIBGSM) $(PACKAGE_DIR)

$(IPKG_GSM_UTILS):
	install -d -m0755 $(IDIR_GSM_UTILS)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(IDIR_GSM_UTILS)/usr/bin/
	$(RSTRIP) $(IDIR_GSM_UTILS)
	$(IPKG_BUILD) $(IDIR_GSM_UTILS) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libgsm.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/gsm.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgsm.a $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgsm.so* $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libgsm.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/gsm.h \
		$(STAGING_DIR)/usr/lib/libgsm.a \
		$(STAGING_DIR)/usr/lib/libgsm.so* \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built

