# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=dropbear
PKG_VERSION:=0.45
PKG_RELEASE:=2
PKG_MD5SUM:=2bcc46e4c239aec982bf36a723dd0b0e

PKG_SOURCE_URL:= \
	http://matt.ucc.asn.au/dropbear/releases/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_DROPBEAR:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_DROPBEAR:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_DROPBEAR:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

$(DL_DIR)/$(PKG_SOURCE):
	 $(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches \*.patch
	$(SED) 's,^/\* #define PKG_MULTI.*,#define PKG_MULTI,g' $(PKG_BUILD_DIR)/options.h
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		autoconf; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
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
		$(DISABLE_LARGEFILE) \
		$(DISABLE_NLS) \
		--with-shared \
		--disable-pam \
		--enable-openpty \
		--enable-syslog \
		--disable-shadow \
		--disable-lastlog \
		--disable-utmp \
		--disable-utmpx \
		--disable-wtmp \
		--disable-wtmpx \
		--disable-loginfunc \
		--disable-pututline \
		--disable-pututxline \
	);
	touch  $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/dropbearmulti: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		LD=$(TARGET_CC) \
		PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" \
		MULTI=1 SCPPROGRESS=1

$(IPKG_DROPBEAR): $(PKG_BUILD_DIR)/dropbearmulti
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_DROPBEAR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_DROPBEAR)/usr/bin
	mkdir -p $(IDIR_DROPBEAR)/usr/sbin
	install -m 755 $(PKG_BUILD_DIR)/dropbearmulti \
		$(IDIR_DROPBEAR)/usr/sbin/dropbear
	$(STRIP) $(IDIR_DROPBEAR)/usr/sbin/dropbear
	ln -sf ../sbin/dropbear $(IDIR_DROPBEAR)/usr/bin/scp
	ln -sf ../sbin/dropbear $(IDIR_DROPBEAR)/usr/bin/ssh
	ln -sf ../sbin/dropbear $(IDIR_DROPBEAR)/usr/bin/dbclient
	ln -sf ../sbin/dropbear $(IDIR_DROPBEAR)/usr/bin/dropbearkey
	ln -sf ../sbin/dropbear $(IDIR_DROPBEAR)/usr/bin/dropbearconvert
	mkdir -p $(IDIR_DROPBEAR)/etc/init.d
	install -m 755 ./files/S50dropbear $(IDIR_DROPBEAR)/etc/init.d/
	$(IPKG_BUILD) $(IDIR_DROPBEAR) $(PACKAGE_DIR)

$(INFO_DROPBEAR): $(IPKG_DROPBEAR)
	$(IPKG) install $(IPKG_DROPBEAR)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_DROPBEAR)
install: $(INFO_DROPBEAR)

clean:
	rm -rf $(PKG_BUILD_DIR) $(IPKG_DROPBEAR)

