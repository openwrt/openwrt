# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := popt
PKG_VERSION := 1.7
PKG_RELEASE := 4
PKG_MD5SUM := 5988e7aeb0ae4dac8d83561265984cc9

PKG_SOURCE_URL := ftp://ftp.rpm.org/pub/rpm/dist/rpm-4.1.x/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT := zcat
PKG_IPK := $(PACKAGE_DIR)/lib$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR := $(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
#	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.patched
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
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
	$(MAKE) -C "$(PKG_BUILD_DIR)"
	touch $(PKG_BUILD_DIR)/.built

$(STAGING_DIR)/usr/lib/libpopt.so: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		DESTDIR="$(STAGING_DIR)" \
		install
	rm -f $(STAGING_DIR)/usr/lib/libpopt.la

$(PKG_IPK): $(STAGING_DIR)/usr/lib/libpopt.so
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) libpopt.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/lib
	cp -fp $(STAGING_DIR)/usr/lib/libpopt.so.* $(PKG_IPK_DIR)/usr/lib/
	$(STRIP) $(PKG_IPK_DIR)/usr/lib/*.so*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/libpopt.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/libpopt.list

clean:
	-$(MAKE) -C "$(PKG_BUILD_DIR)" \
		DESTDIR="$(STAGING_DIR)" \
		uninstall
	rm -rf \
		$(PKG_BUILD_DIR) \
		$(PKG_IPK) \
