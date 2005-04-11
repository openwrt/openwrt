# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := pcre
PKG_VERSION := 5.0
PKG_RELEASE := 3
PKG_MD5SUM := 53976b62032fd5c76678274e5fd196b4

PKG_SOURCE_URL := ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/ \
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
		--enable-utf8 \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		CC_FOR_BUILD="$(HOSTCC)" \
		LINK_FOR_BUILD="$(HOSTCC)" \
		CFLAGS_FOR_BUILD="" \
		dftables
	$(MAKE) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
	  
	touch $(PKG_BUILD_DIR)/.built

$(STAGING_DIR)/usr/lib/libpcre.so: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		DESTDIR="$(STAGING_DIR)" \
		install
	rm -f $(STAGING_DIR)/usr/lib/libpcre*.la

$(PKG_IPK): $(STAGING_DIR)/usr/lib/libpcre.so
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) libpcre.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/lib
	cp -fp $(STAGING_DIR)/usr/lib/libpcre*.so.* $(PKG_IPK_DIR)/usr/lib/
	$(STRIP) $(PKG_IPK_DIR)/usr/lib/*.so*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/libpcre.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/libpcre.list

clean:
	rm -rf \
	  $(STAGING_DIR)/usr/bin/pcre-config \
	  $(STAGING_DIR)/usr/include/pcre* \
	  $(STAGING_DIR)/usr/lib/libpcre* \
	  $(STAGING_DIR)/usr/lib/pkgconfig/libpcre.pc \
	  $(STAGING_DIR)/usr/share/man/man*/pcre* \
	  $(PKG_BUILD_DIR) \
	  $(PKG_IPK) \
