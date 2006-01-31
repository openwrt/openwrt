# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=dropbear
PKG_VERSION:=0.47
PKG_RELEASE:=1
PKG_MD5SUM:=cf634614d52278d44dfd9c224a438bf2

PKG_SOURCE_URL:=http://matt.ucc.asn.au/dropbear/releases/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,DROPBEAR,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,DBCONVERT,dropbearconvert,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	$(SED) 's,^/\* #define PKG_MULTI.*,#define PKG_MULTI,g' $(PKG_BUILD_DIR)/options.h
	$(SED) 's,^#define DO_HOST_LOOKUP,/* & */,g' $(PKG_BUILD_DIR)/options.h
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
		--disable-zlib \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		LD=$(TARGET_CC) \
		PROGRAMS="dropbear dbclient dropbearkey scp" \
		MULTI=1 SCPPROGRESS=1
	$(MAKE) -C $(PKG_BUILD_DIR) \
		LD=$(TARGET_CC) \
		PROGRAMS="dropbearconvert"
	touch $@
	
$(IPKG_DBCONVERT):
	mkdir -p $(IDIR_DBCONVERT)/usr/bin
	install -m 755 $(PKG_BUILD_DIR)/dropbearconvert \
		$(IDIR_DBCONVERT)/usr/bin/dropbearconvert
	$(IPKG_BUILD) $(IDIR_DBCONVERT) $(PACKAGE_DIR)

$(IPKG_DROPBEAR):
	mkdir -p $(IDIR_DROPBEAR)/usr/bin
	mkdir -p $(IDIR_DROPBEAR)/usr/sbin
	install -m 755 $(PKG_BUILD_DIR)/dropbearmulti \
		$(IDIR_DROPBEAR)/usr/sbin/dropbear
	$(STRIP) $(IDIR_DROPBEAR)/usr/sbin/dropbear
	ln -sf ../sbin/dropbear $(IDIR_DROPBEAR)/usr/bin/scp
	ln -sf ../sbin/dropbear $(IDIR_DROPBEAR)/usr/bin/ssh
	ln -sf ../sbin/dropbear $(IDIR_DROPBEAR)/usr/bin/dbclient
	ln -sf ../sbin/dropbear $(IDIR_DROPBEAR)/usr/bin/dropbearkey
	mkdir -p $(IDIR_DROPBEAR)/etc/init.d
	install -m 755 ./files/S50dropbear $(IDIR_DROPBEAR)/etc/init.d/
	$(IPKG_BUILD) $(IDIR_DROPBEAR) $(PACKAGE_DIR)
