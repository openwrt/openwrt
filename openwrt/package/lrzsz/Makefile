include $(TOPDIR)/rules.mk

PKG_NAME:=lrzsz
PKG_VERSION:=0.12.20
PKG_RELEASE:=1
PKG_MD5SUM:=b5ce6a74abc9b9eb2af94dffdfd372a4

PKG_SOURCE_URL:=http://www.ohse.de/uwe/releases/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LRZSZ,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--prefix=/usr \
			--without-libiconv-prefix \
			--without-libintl-prefix \
			--disable-nls \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)/usr/bin
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		prefix="$(PKG_INSTALL_DIR)/usr"
	cp -fpR $(PKG_BUILD_DIR)/src/lrz $(PKG_INSTALL_DIR)/usr/bin
	cp -fpR $(PKG_BUILD_DIR)/src/lsz $(PKG_INSTALL_DIR)/usr/bin
	touch $@

$(IPKG_LRZSZ):
	install -d -m0755 $(IDIR_LRZSZ)/usr/bin
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/lrz $(IDIR_LRZSZ)/usr/bin
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/lsz $(IDIR_LRZSZ)/usr/bin
	(cd $(IDIR_LRZSZ)/usr/bin;\
	 ln -s lrz lrx;\
	 ln -s lrz lrb;\
	 ln -s lsz lsx;\
	 ln -s lsz lsb;);
	$(RSTRIP) $(IDIR_LRZSZ)
	$(IPKG_BUILD) $(IDIR_LRZSZ) $(PACKAGE_DIR)

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built

