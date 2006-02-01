# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libmad
PKG_VERSION:=0.15.1b
PKG_RELEASE:=1
PKG_MD5SUM:=1be543bc30c56fb6bea1d7bf6a64e66c

PKG_SOURCE_URL:=@SF/mad \
	ftp://ftp.mars.org/pub/mpeg/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBMAD,libmad,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

FPM:=default
ifeq ($(ARCH),i386)
FPM:=intel
endif
ifeq ($(ARCH),mipsel)
FPM:=mips
endif

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(strip $(TARGET_CFLAGS))" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		PKG_CONFIG_PATH="$(STAGING_DIR)/usr/lib/pkgconfig" \
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
			--enable-static \
			--disable-rpath \
			--with-gnu-ld \
			--enable-fpm=$(FPM) \
			--disable-debugging \
			--enable-speed \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBMAD):
	install -m0755 -d $(IDIR_LIBMAD)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmad.so.* $(IDIR_LIBMAD)/usr/lib/
	$(RSTRIP) $(IDIR_LIBMAD)
	$(IPKG_BUILD) $(IDIR_LIBMAD) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libmad.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/mad.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmad.a $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmad.so* $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libmad.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/mad.h \
		$(STAGING_DIR)/usr/lib/libmad.a \
		$(STAGING_DIR)/usr/lib/libmad.so* \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built

