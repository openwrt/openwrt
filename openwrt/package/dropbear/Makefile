# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=dropbear
PKG_VERSION:=0.48.1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://matt.ucc.asn.au/dropbear/releases/
PKG_MD5SUM:=ca8e53a766faec831882831364568421
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

define Package/dropbear
SECTION:=base
CATEGORY:=Base system
DEFAULT:=y
TITLE:=Small SSH 2 client/server
DESCRIPTION:=A small SSH 2 server/client designed for small memory environments.
URL:=http://matt.ucc.asn.au/dropbear/
endef

define Package/dropbearconvert
$(call Package/dropbear)
DEFAULT:=m
TITLE:=Utility for converting SSH keys
DESCRIPTION:=Utility for converting SSH keys
DEPENDS:=zlib
endef

define Build/Configure
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
endef

define Buid/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		LD=$(TARGET_CC) \
		PROGRAMS="dropbear dbclient dropbearkey scp" \
		MULTI=1 SCPPROGRESS=1
	$(MAKE) -C $(PKG_BUILD_DIR) \
		LD=$(TARGET_CC) \
		PROGRAMS="dropbearconvert"
endef
	
define Package/dropbear/install
	mkdir -p $(IDIR_DROPBEAR)/usr/bin
	mkdir -p $(IDIR_DROPBEAR)/usr/sbin
	install -m 755 $(PKG_BUILD_DIR)/dropbearmulti \
		$(IDIR_DROPBEAR)/usr/sbin/dropbear
	ln -sf ../sbin/dropbear $(IDIR_DROPBEAR)/usr/bin/scp
	ln -sf ../sbin/dropbear $(IDIR_DROPBEAR)/usr/bin/ssh
	ln -sf ../sbin/dropbear $(IDIR_DROPBEAR)/usr/bin/dbclient
	ln -sf ../sbin/dropbear $(IDIR_DROPBEAR)/usr/bin/dropbearkey
	mkdir -p $(IDIR_DROPBEAR)/etc/init.d
	install -m 755 ./files/S50dropbear $(IDIR_DROPBEAR)/etc/init.d/
endef

define Package/dropbearconvert/install
	mkdir -p $(IDIR_DBCONVERT)/usr/bin
	install -m 755 $(PKG_BUILD_DIR)/dropbearconvert \
		$(IDIR_DBCONVERT)/usr/bin/dropbearconvert
endef

$(eval $(call BuildPackage,dropbear))
$(eval $(call BuildPackage,dropbearconvert))
