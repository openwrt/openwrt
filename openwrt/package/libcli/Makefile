# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libcli
PKG_VERSION:=1.8.5
PKG_RELEASE:=1
PKG_MD5SUM:=0fdd30df5a8c1388a4549751ba61247a

PKG_SOURCE_URL:=@SF/libcli
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBCLI,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) CC=$(TARGET_CC) LD=$(TARGET_CC) \
		DESTDIR=$(PKG_INSTALL_DIR) \
		-C $(PKG_BUILD_DIR) install
	touch $@

$(IPKG_LIBCLI):
	install -d -m0755 $(IDIR_LIBCLI)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcli* $(IDIR_LIBCLI)/usr/lib
	$(RSTRIP) $(IDIR_LIBCLI)
	$(IPKG_BUILD) $(IDIR_LIBCLI) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libcli.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libcli.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcli.so* $(STAGING_DIR)/usr/lib/

install-dev: $(STAGING_DIR)/usr/lib/libcli.so

uninstall-dev:
	rm -rf \
	  $(STAGING_DIR)/usr/include/libcli.h \
	  $(STAGING_DIR)/usr/lib/libcli.so*

compile-targets: install-dev
clean-targets: uninstall-dev

