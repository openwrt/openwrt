# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ctorrent
PKG_VERSION:=1.3.4-dnh2
PKG_RELEASE:=1
PKG_MD5SUM:=6635e2d0d0ee21faeb85d95cd93c137b

PKG_SOURCE_URL:=http://www.rahul.net/dholmes/ctorrent/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-dnh2
PKG_CAT:=zcat
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,CTORRENT,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include -I$(STAGING_DIR)/usr/include/openssl" \
		LDFLAGS="-static -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		--program-prefix="" \
		--with-gnu-ld \
	);
	touch $@
	
$(PKG_BUILD_DIR)/.built:
	$(MAKE) STAGING_DIR=$(STAGING_DIR) DESTDIR=$(PKG_INSTALL_DIR) -C $(PKG_BUILD_DIR) install
	
$(IPKG_CTORRENT):
	mkdir -p $(IDIR_CTORRENT)
	$(CP) $(PKG_INSTALL_DIR)/* $(IDIR_CTORRENT)/
	$(RSTRIP) $(IDIR_CTORRENT)/
	$(IPKG_BUILD) $(IDIR_CTORRENT) $(PACKAGE_DIR)
