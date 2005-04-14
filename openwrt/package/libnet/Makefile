# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libnet
PKG_VERSION:=1.0.2a
PKG_RELEASE:=7
PKG_MD5SUM:=ddf53f0f484184390e8c2a1bd0853667

PKG_SOURCE_URL:=\
	http://www.packetfactory.net/libnet/dist/deprecated/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/Libnet-$(PKG_VERSION)

IPKG_LIBNET:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_LIBNET:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_LIBNET:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.cache config.status; \
		touch configure.in; \
		touch include.m4; \
		touch aclocal.m4; \
		touch Makefile.in; \
		touch configure; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		ac_libnet_have_pf_packet=yes \
		ac_cv_lbl_unaligned_fail=no \
		ac_cv_libnet_endianess=lil \
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
		--with-pf_packet=yes \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)"
	touch $(PKG_BUILD_DIR)/.built

$(STAGING_DIR)/usr/lib/libnet.so: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		install
	rm -rf $(STAGING_DIR)/usr/lib/libnet.la

$(IPKG_LIBNET): $(STAGING_DIR)/usr/lib/libnet.so
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_LIBNET) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_LIBNET)/usr/lib
	cp -fp $(STAGING_DIR)/usr/lib/libnet.so.* $(IDIR_LIBNET)/usr/lib/
	$(STRIP) $(IDIR_LIBNET)/usr/lib/lib*.so*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_LIBNET) $(PACKAGE_DIR)

$(INFO_LIBNET): $(IPKG_LIBNET)
	$(IPKG) install $(IPKG_LIBNET)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_LIBNET)
install: $(INFO_LIBNET)

clean:
	-$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		uninstall
	rm -rf $(PKG_BUILD_DIR) $(IPKG_LIBNET)
