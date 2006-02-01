# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libosip2
PKG_VERSION:=2.2.0
PKG_RELEASE:=1
PKG_MD5SUM:=f1237af0250c59b318bd64a59f5750ea

PKG_SOURCE_URL:=@GNU/osip
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBOSIP2,libosip2,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(strip $(TARGET_CFLAGS))" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
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
		  --with-gnu-ld \
		  --disable-debug \
		  --disable-trace \
		  --enable-pthread \
		  --enable-semaphore \
		  --enable-ntimer \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBOSIP2):
	install -m0755 -d $(IDIR_LIBOSIP2)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libosip{,parser}2.so.* $(IDIR_LIBOSIP2)/usr/lib/
	$(RSTRIP) $(IDIR_LIBOSIP2)
	$(IPKG_BUILD) $(IDIR_LIBOSIP2) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libosip2.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/osip{,parser}2 $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libosip{,parser}2.{a,so*} $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libosip2.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libosip2.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/osip{,parser}2 \
		$(STAGING_DIR)/usr/lib/libosip{,parser}2.{a,so*} \
		$(STAGING_DIR)/usr/lib/pkgconfig/libosip2.pc \

compile-targets: install-dev
clean-targets: uninstall-dev
