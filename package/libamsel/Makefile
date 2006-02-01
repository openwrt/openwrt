# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libamsel
PKG_VERSION:=20040621-cvs
PKG_RELEASE:=1
PKG_MD5SUM:=2077e620047404881a65d39c53c9917e

PKG_SOURCE_URL:=ftp://ftp.amselinux.net/source/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBAMSEL,libamsel,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		LDIR=$(PKG_BUILD_DIR) \
		PREFIX="$(PKG_INSTALL_DIR)/usr" \
		all install
	touch $@

$(IPKG_LIBAMSEL):
	install -d -m0755 $(IDIR_LIBAMSEL)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(IDIR_LIBAMSEL)/usr/lib
	$(RSTRIP) $(IDIR_LIBAMSEL)
	$(IPKG_BUILD) $(IDIR_LIBAMSEL) $(PACKAGE_DIR)


$(STAGING_DIR)/usr/lib/libamsel.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/amsel/compat.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/include/compat
	$(CP) $(PKG_INSTALL_DIR)/usr/include/amsel/compat/queue.h $(STAGING_DIR)/usr/include/compat
	mkdir -p $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libamsel.so $(STAGING_DIR)/usr/lib/
	touch $(STAGING_DIR)/usr/lib/libamsel.so
	
install-dev: $(STAGING_DIR)/usr/lib/libamsel.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/compat.h \
		$(STAGING_DIR)/usr/include/compat \
		$(STAGING_DIR)/usr/lib/libamsel.so \

compile-targets: install-dev
clean-targets: uninstall-dev

