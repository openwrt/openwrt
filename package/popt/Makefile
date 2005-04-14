# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=popt
PKG_VERSION:=1.7
PKG_RELEASE:=4
PKG_MD5SUM:=5988e7aeb0ae4dac8d83561265984cc9

PKG_SOURCE_URL:=\
	ftp://ftp.rpm.org/pub/rpm/dist/rpm-4.1.x/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_LIBPOPT:=$(PACKAGE_DIR)/lib$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_LIBPOPT:=$(PKG_BUILD_DIR)/ipkg/lib$(PKG_NAME)
INFO_LIBPOPT:=$(IPKG_STATE_DIR)/info/lib$(PKG_NAME).list

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
#	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.cache config.status; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
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
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
		--enable-shared \
		--enable-static \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR)
	touch $(PKG_BUILD_DIR)/.built

$(STAGING_DIR)/usr/lib/libpopt.so: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		install
	rm -f $(STAGING_DIR)/usr/lib/libpopt.la

$(IPKG_LIBPOPT): $(STAGING_DIR)/usr/lib/libpopt.so
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_LIBPOPT) lib$(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_LIBPOPT)/usr/lib
	cp -fp $(STAGING_DIR)/usr/lib/libpopt.so.* $(IDIR_LIBPOPT)/usr/lib/
	$(STRIP) $(IDIR_LIBPOPT)/usr/lib/*.so*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_LIBPOPT) $(PACKAGE_DIR)

$(INFO_LIBPOPT): $(IPKG_LIBPOPT)
	$(IPKG) install $(IPKG_LIBPOPT)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_LIBPOPT)
install: $(INFO_LIBPOPT)

clean:
	-$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		uninstall
	rm -rf $(PKG_BUILD_DIR) $(IPKG_LIBPOPT)
