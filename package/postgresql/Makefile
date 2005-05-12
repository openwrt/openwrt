# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=postgresql
PKG_VERSION:=7.4.7
PKG_RELEASE:=3
PKG_MD5SUM:=32dac2916d16287d95e0c958a75161fa

PKG_SOURCE_URL:=\
	ftp://ftp3.us.postgresql.org/pub/postgresql/source/v$(PKG_VERSION)/ \
	ftp://ftp.br.postgresql.org/pub/PostgreSQL/source/v$(PKG_VERSION)/ \
	ftp://ftp.au.postgresql.org/pub/postgresql/source/v$(PKG_VERSION)/ \
	ftp://ftp.tw.postgresql.org/pub/postgresql/source/v$(PKG_VERSION)/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_LIBPQ:=$(PACKAGE_DIR)/libpq_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_LIBPQ:=$(PKG_BUILD_DIR)/ipkg
INFO_LIBPQ:=$(IPKG_STATE_DIR)/info/libpq.list

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
#	./configure advertise "--infodir", but does not support it, replaced with "--docdir"
	(cd $(PKG_BUILD_DIR); rm -rf config.cache config.status; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
		CPPFLAGS="$$CPPFLAGS -I$(STAGING_DIR)/usr/include" \
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
		--docdir=/usr/share/info \
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
		--disable-integer-datetimes \
		--disable-rpath \
		--without-java \
		--without-krb4 \
		--without-krb5 \
		--without-openssl \
		--without-pam \
		--without-perl \
		--without-python \
		--without-readline \
		--without-rendezvous \
		--without-tcl \
		--without-tk \
		--with-zlib="yes" \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C "$(PKG_BUILD_DIR)/src" \
		$(TARGET_CONFIGURE_OPTS) \
		submake-libpq
	touch $(PKG_BUILD_DIR)/.built

$(STAGING_DIR)/usr/lib/libpq.so: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C "$(PKG_BUILD_DIR)/src" \
		DESTDIR="$(STAGING_DIR)" \
		install-all-headers
	$(MAKE) -C "$(PKG_BUILD_DIR)/src/include" \
		DESTDIR="$(STAGING_DIR)" \
		install
	$(MAKE) -C "$(PKG_BUILD_DIR)/src/interfaces/libpq" \
		DESTDIR="$(STAGING_DIR)" \
		install
	mkdir -p $(STAGING_DIR)/usr/lib
	cp -a $(PKG_BUILD_DIR)/src/interfaces/libpq/libpq.so* $(STAGING_DIR)/usr/lib/
	cp $(PKG_BUILD_DIR)/src/interfaces/libpq/libpq.a $(STAGING_DIR)/usr/lib/

$(IPKG_LIBPQ): $(STAGING_DIR)/usr/lib/libpq.so
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_LIBPQ) ./ipkg/libpq.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_LIBPQ)/usr/lib
	cp -fpR $(STAGING_DIR)/usr/lib/libpq.so.* $(IDIR_LIBPQ)/usr/lib/
	$(STRIP) $(IDIR_LIBPQ)/usr/lib/*.so*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_LIBPQ) $(PACKAGE_DIR)

$(INFO_LIBPQ): $(IPKG_LIBPQ)
	$(IPKG) install $(IPKG_LIBPQ)

prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_LIBPQ)
install: $(INFO_LIBPQ)

clean:
	-$(MAKE) -C "$(PKG_BUILD_DIR)" \
		DESTDIR="$(STAGING_DIR)" \
		uninstall
	rm -rf $(PKG_BUILD_DIR) $(IPKG_LIBPQ)
