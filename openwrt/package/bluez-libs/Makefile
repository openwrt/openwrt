# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=bluez-libs
PKG_VERSION:=2.17
PKG_RELEASE:=2
PKG_MD5SUM:=7b81dc86a48b89600eee235c6c940c51

PKG_SOURCE_URL:=http://bluez.sourceforge.net/download
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,BLUEZ_LIBS,bluez-libs,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.source: $(DL_DIR)/$(PKG_SOURCE)
	zcat $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(BLUEZLIBS_DIR)/.source

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		sys_lib_dlsearch_path_spec="$(STAGING_DIR)/lib $(STAGING_DIR)/usr/lib" \
		sys_lib_search_path_spec="$(STAGING_DIR)/lib $(STAGING_DIR)/usr/lib" \
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
		  --without-x \
		  --with-png=$(STAGING_DIR)/usr \
	)
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_BLUEZ_LIBS):
	install -d -m0755 $(IDIR_BLUEZ_LIBS)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libbluetooth.so.* $(IDIR_BLUEZ_LIBS)/usr/lib/
	$(RSTRIP) $(IDIR_BLUEZ_LIBS)
	$(IPKG_BUILD) $(IDIR_BLUEZ_LIBS) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libbluetooth.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include/bluetooth
	cp -fpR $(PKG_INSTALL_DIR)/usr/include/bluetooth/*.h $(STAGING_DIR)/usr/include/bluetooth
	mkdir -p $(STAGING_DIR)/usr/lib
	cp -fp $(PKG_INSTALL_DIR)/usr/lib/libbluetooth.{a,so*} $(STAGING_DIR)/usr/lib/

install-dev: $(STAGING_DIR)/usr/lib/libbluetooth.so

uninstall-dev:
	rm -rf $(STAGING_DIR)/usr/lib/libbluetooth.{a,so*}

compile: install-dev
clean: uninstall-dev
