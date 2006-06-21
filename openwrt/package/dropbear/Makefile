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

include $(INCLUDE_DIR)/package.mk

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
TITLE:=Utility for converting SSH keys
DEFAULT:=m if ALL
DESCRIPTION:=Utility for converting SSH keys
endef

define Build/Configure
	$(SED) 's,^/\* #define PKG_MULTI.*,#define PKG_MULTI,g' $(PKG_BUILD_DIR)/options.h
	$(SED) 's,^#define DO_HOST_LOOKUP,/* & */,g' $(PKG_BUILD_DIR)/options.h
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		LD=$(TARGET_CC) \
		PROGRAMS="dropbear dbclient dropbearkey scp" \
		MULTI=1 SCPPROGRESS=1
	$(MAKE) -C $(PKG_BUILD_DIR) \
		LD=$(TARGET_CC) \
		PROGRAMS="dropbearconvert"
endef
	
define Package/dropbear/install
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/dropbearmulti \
		$(1)/usr/sbin/dropbear
	install -d -m0755 $(1)/usr/bin
	ln -sf ../sbin/dropbear $(1)/usr/bin/scp
	ln -sf ../sbin/dropbear $(1)/usr/bin/ssh
	ln -sf ../sbin/dropbear $(1)/usr/bin/dbclient
	ln -sf ../sbin/dropbear $(1)/usr/bin/dropbearkey
	install -d -m0755 $(1)/etc/init.d
	install -m0755 ./files/dropbear.init $(1)/etc/init.d/S50dropbear
endef

define Package/dropbearconvert/install
	install -d -m0755 $(1)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/dropbearconvert \
		$(1)/usr/bin/dropbearconvert
endef

$(eval $(call BuildPackage,dropbear))
$(eval $(call BuildPackage,dropbearconvert))
