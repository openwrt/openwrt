# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=serdisplib
PKG_VERSION:=1.96
PKG_RELEASE:=1
PKG_MD5SUM:=b074e02d8d63f34cba8ec45c23f63f78

PKG_SOURCE_URL:=@SF/serdisplib
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,SERDISPLIB,serdisplib,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		ac_cv_func_malloc_0_nonnull=yes \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--program-prefix="" \
			--program-suffix="" \
			--prefix=/usr \
			--exec-prefix=/usr \
			--bindir=/usr/bin \
			--datadir=/usr/share \
			--includedir=/usr/include \
			--infodir=/usr/share/info \
			--libdir=/usr/lib \
			--libexecdir=/usr/lib \
			--localstatedir=/var \
			--mandir=/usr/share/man \
			--sbindir=/usr/sbin \
			--sysconfdir=/etc \
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
			--enable-shared \
			--disable-static \
			--with-gnu-ld \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) all
	touch $@

$(IPKG_SERDISPLIB):
	install -d -m0755 $(IDIR_SERDISPLIB)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/lib/libserdisp.so.* $(IDIR_SERDISPLIB)/usr/lib
	$(RSTRIP) $(IDIR_SERDISPLIB)
	$(IPKG_BUILD) $(IDIR_SERDISPLIB) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libserdisp.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_BUILD_DIR)/include/serdisplib $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/lib/libserdisp.{a,so*} $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libserdisp.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/serdisplib \
		$(STAGING_DIR)/usr/lib/libserdisp.{a,so*} \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
