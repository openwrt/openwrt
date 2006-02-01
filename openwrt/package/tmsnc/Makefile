include $(TOPDIR)/rules.mk

PKG_NAME:=tmsnc
PKG_VERSION:=0.3.1
PKG_RELEASE:=1
PKG_MD5SUM:=3b30e5dab0b1b679d2b945c3a5713636
PKG_SOURCE_URL:=@SF/tmsnc
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,TMSNC,tmsnc,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		ac_cv_func_malloc_0_nonnull=yes \
		ac_cv_func_realloc_0_nonnull=yes \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		./configure \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --prefix=/usr \
		  --exec-prefix=/usr \
		  --bindir=/usr/bin \
		  --sbindir=/usr/sbin \
		  --libexecdir=/usr/lib \
		  --sysconfdir=/etc \
		  --datadir=/usr/share \
		  --localstatedir=/var \
		  --mandir=/usr/man \
		  --infodir=/usr/info \
		  $(DISABLE_NLS) \
		  --with-libiconv-prefix=$(STAGING_DIR)/usr \
		  --with-openssl=$(STAGING_DIR)/usr \
		  --with-ncurses=$(STAGING_DIR) \
		  --with-gnu-ld \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	rm -rf $(PKG_INSTALL_DIR)/usr/man
	touch $@

$(IPKG_TMSNC):
	mkdir -p $(IDIR_TMSNC)
	$(CP) $(PKG_INSTALL_DIR)/* $(IDIR_TMSNC)/
	$(RSTRIP) $(IDIR_TMSNC)
	$(IPKG_BUILD) $(IDIR_TMSNC) $(PACKAGE_DIR)
