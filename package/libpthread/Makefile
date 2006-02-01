# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libpthread
PKG_VERSION:=0.9.28
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBPTHREAD,libpthread,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $(PKG_BUILD_DIR)
	touch $@

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	touch $@

$(IPKG_LIBPTHREAD): $(PKG_BUILD_DIR)/.built
	mkdir -p $(IDIR_LIBPTHREAD)/lib
	$(CP) $(STAGING_DIR)/lib/libpthread.so.* $(IDIR_LIBPTHREAD)/lib/
	$(CP) $(STAGING_DIR)/lib/libpthread-*.so $(IDIR_LIBPTHREAD)/lib/
	$(RSTRIP) $(IDIR_LIBPTHREAD)
	$(IPKG_BUILD) $(IDIR_LIBPTHREAD) $(PACKAGE_DIR)

