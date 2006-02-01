# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=cgilib
PKG_VERSION:=0.5
PKG_RELEASE:=1
PKG_MD5SUM:=5187ba11b2ec165b6c5b6629087733b0

PKG_SOURCE_URL:=http://www.infodrom.org/projects/cgilib/download
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,CGILIB,cgilib,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		OFLAGS="$(TARGET_CFLAGS)" \
		all
	touch $@

$(IPKG_CGILIB):
	install -d -m0755 $(IDIR_CGILIB)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libcgi.so.* $(IDIR_CGILIB)/usr/lib/
	$(RSTRIP) $(IDIR_CGILIB)
	$(IPKG_BUILD) $(IDIR_CGILIB) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libcgi.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_BUILD_DIR)/cgi.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libcgi.{a,so*} $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libcgi.so
uninstall-dev:
	rm -rf \
	  $(STAGING_DIR)/usr/include/cgi.h \
	  $(STAGING_DIR)/usr/lib/libcgi.{a,so*} \

compile-targets: install-dev
clean-targets: uninstall-dev
