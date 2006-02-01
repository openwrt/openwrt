# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libvorbis
PKG_VERSION:=1.1.2
PKG_RELEASE:=1
PKG_MD5SUM:=37847626b8e1b53ae79a34714c7b3211

PKG_SOURCE_URL:=http://downloads.xiph.org/releases/vorbis/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBVORBIS,libvorbis,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

# NOTE: check ./toolchain/libnotimpl
# it implements those functions missing from the version of uClibc we're currently using
# for libvorbis, they are cosf/sinf

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(strip $(TARGET_CFLAGS))" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		LIBS="-lm -logg -lnotimpl" \
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
			--with-ogg="$(STAGING_DIR)/usr" \
			--with-ogg-includes="$(STAGING_DIR)/usr/include" \
			--with-ogg-libraries="$(STAGING_DIR)/usr/lib" \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBVORBIS):
	install -m0755 -d $(IDIR_LIBVORBIS)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libvorbis.so.* $(IDIR_LIBVORBIS)/usr/lib/
	$(RSTRIP) $(IDIR_LIBVORBIS)
	$(IPKG_BUILD) $(IDIR_LIBVORBIS) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libvorbis.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/vorbis $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libvorbis.a $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libvorbis{,enc,file}.so* $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/vorbis{,enc,file}.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	$(SED) 's,-I$${includedir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/vorbis{,enc,file}.pc
	$(SED) 's,-L$${libdir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/vorbis{,enc,file}.pc
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libvorbis.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/vorbis \
		$(STAGING_DIR)/usr/lib/libvorbis{,enc,file}.a \
		$(STAGING_DIR)/usr/lib/libvorbis{,enc,file}.so* \
		$(STAGING_DIR)/usr/lib/pkgconfig/vorbis{,enc,file}.pc \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built

