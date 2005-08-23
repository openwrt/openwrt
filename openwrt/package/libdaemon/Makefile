# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libdaemon
PKG_VERSION:=0.8
PKG_RELEASE:=1
PKG_MD5SUM:=49cc7db480c6d7b8ca88b0db8ac275e7

PKG_SOURCE_URL:=http://0pointer.de/lennart/projects/libdaemon/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBDAEMON,libdaemon,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
		ac_cv_func_setpgrp_void=yes \
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
		  --disable-lynx \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBDAEMON):
	install -d -m0755 $(IDIR_LIBDAEMON)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libdaemon.so.* $(IDIR_LIBDAEMON)/usr/lib/
	$(RSTRIP) $(IDIR_LIBDAEMON)
	$(IPKG_BUILD) $(IDIR_LIBDAEMON) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libdaemon.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	cp -fpR $(PKG_INSTALL_DIR)/usr/include/libdaemon $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libdaemon.a $(STAGING_DIR)/usr/lib/
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libdaemon.so* $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libdaemon.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	$(SED) 's,-I$${includedir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/libdaemon.pc
	$(SED) 's,-L$${libdir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/libdaemon.pc
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libdaemon.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/libdaemon \
		$(STAGING_DIR)/usr/lib/libdaemon.a \
		$(STAGING_DIR)/usr/lib/libdaemon.so* \
		$(STAGING_DIR)/usr/lib/pkgconfig/libdaemon.pc \

compile: install-dev
clean: uninstall-dev

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built

