# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=vtun
PKG_VERSION:=2.6
PKG_RELEASE:=1
PKG_MD5SUM:=309534fd03c5d13a19c43916f61f4bbf

PKG_SOURCE_URL:=@SF/vtun
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,VTUN,vtun,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR) ; rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LIBS="-L$(STAGING_DIR)/usr/lib" \
		BLOWFISH_HDR_DIR="$(STAGING_DIR)/usr/include/openssl" \
		LZO_HDR_DIR="$(STAGING_DIR)/usr/include" \
		SSL_HDR_DIR="$(STAGING_DIR)/usr/include/openssl" \
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
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		vtund
	touch $@

$(IPKG_VTUN):
	install -m0755 -d $(IDIR_VTUN)/etc
	install -m0600 $(PKG_BUILD_DIR)/vtund.conf $(IDIR_VTUN)/etc/
	install -m0755 -d $(IDIR_VTUN)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/vtund $(IDIR_VTUN)/usr/sbin/
	$(RSTRIP) $(IDIR_VTUN)
	$(IPKG_BUILD) $(IDIR_VTUN) $(PACKAGE_DIR)
