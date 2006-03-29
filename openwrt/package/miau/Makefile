# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=miau
PKG_VERSION:=0.6.0.2
PKG_RELEASE:=1
PKG_MD5SUM:=1b78d219a03655399c165eeb021cddd8

PKG_SOURCE_URL:= @SF/miau
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MIAU,miau,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		ac_cv_func_malloc_0_nonnull=yes \
		ac_cv_func_realloc_0_nonnull=yes \
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
			--bindir=/usr/bin \
			--sysconfdir=/etc \
			--enable-automode \
			--enable-releasenick \
			--enable-uptime \
			--enable-privlog \
			--enable-onconnect \
			$(DISABLE_NLS) \
			$(DISABLE_LARGEFILE) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	touch $@

$(IPKG_MIAU): 
	install -d -m0755 $(IDIR_MIAU)/usr/bin  	
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/$(PKG_NAME) $(IDIR_MIAU)/usr/bin/
	install -d -m0755 $(IDIR_MIAU)/etc/init.d
	install -d -m0755 $(IDIR_MIAU)/etc/default
	install -d -m0755 $(IDIR_MIAU)/etc/miau
	install -m0755 ./files/S50miau $(IDIR_MIAU)/etc/init.d/S50miau
	install -m0644 ./files/miau $(IDIR_MIAU)/etc/default/miau
	install -m0644 ./files/miaurc $(IDIR_MIAU)/etc/miau/miaurc
	$(RSTRIP) $(IDIR_MIAU)
	$(IPKG_BUILD) $(IDIR_MIAU) $(PACKAGE_DIR)
