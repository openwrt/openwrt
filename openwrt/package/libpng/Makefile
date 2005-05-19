# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libpng
PKG_VERSION:=1.2.8
PKG_RELEASE:=1
PKG_MD5SUM:=e5a39003eed16103cbbd3b6a8bc6b1f9

PKG_SOURCE_URL:=@SF/libpng
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-config.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)-config

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBPNG,libpng,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--prefix=$(STAGING_DIR)/usr \
			--with-zlib=$(STAGING_DIR)/usr/ \
			--enable-shared \
			--enable-static \
			--with-zlib=$(STAGING_DIR)/usr/ \
			$(DISABLE_NLS) \
			$(DISABLE_LARGEFILE) \
	)
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) all

$(IPKG_LIBPNG):
	install -d -m0755 $(IDIR_LIBPNG)/usr/lib
	cp -fpR $(PKG_BUILD_DIR)/.libs/libpng{,12}.so.* $(IDIR_LIBPNG)/usr/lib/
	$(RSTRIP) $(IDIR_LIBPNG)
	$(IPKG_BUILD) $(IDIR_LIBPNG) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libpng.so: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C $(PKG_BUILD_DIR) all

install-dev: $(STAGING_DIR)/usr/lib/libpng.so

uninstall-dev:
	rm -rf \
	  $(STAGING_DIR)/usr/bin/libpng{,12}-config \
	  $(STAGING_DIR)/usr/include/png{,conf}.h \
	  $(STAGING_DIR)/usr/include/libpng \
	  $(STAGING_DIR)/usr/lib/libpng{,12}.{a,so*} \
	  $(STAGING_DIR)/usr/lib/pkgconfig/libpng{,12}.pc

compile: install-dev
clean: uninstall-dev

