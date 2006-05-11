# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ipsec-tools
PKG_VERSION:=0.6.4
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/ipsec-tools
PKG_MD5SUM:=d0242a943c82c0cbf28005966ff35e21
PKG_CAT:=bzcat

include $(TOPDIR)/package/rules.mk

define Package/ipsec-tools
SECTION:=base
CATEGORY:=Network
DEFAULT:=y
TITLE:=IPsec management tools
DESCRIPTION:=IPsec management tools
URL:=http://ipsec-tools.sourceforge.net/
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
		touch configure.ac; \
		touch aclocal.m4; \
		touch Makefile.in; \
		touch config.h.in; \
		touch configure; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
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
			--with-gnu-ld \
			--with-kernel-headers="$(LINUX_DIR)/include" \
			--without-readline \
			--with-openssl="$(STAGING_DIR)/usr" \
			--without-libradius \
			--without-libpam \
	);
endef

define Package/ipsec-tools/install
	install -d -m0755 $(1)/etc
	install -d -m0755 $(1)/usr/lib/
	install -d -m0755 $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/src/racoon/samples/racoon.conf $(1)/etc/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libipsec.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libracoon.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/plainrsa-gen $(1)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/racoon $(1)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/racoonctl $(1)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/setkey $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ipsec-tools))
