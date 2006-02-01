# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=fping
PKG_VERSION:=2.4b2_to-ipv6
PKG_RELEASE:=1
PKG_MD5SUM:=3ad516765514249a40d3c5b6caab812a

PKG_SOURCE_URL:=http://www.fping.com/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,FPING,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -j1 CC=$(TARGET_CC) CFLAGS="$(TARGET_CFLAGS)" -C $(PKG_BUILD_DIR) clean $(PKG_NAME)
	mv $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_BUILD_DIR)/$(PKG_NAME)4
	$(MAKE) -j1 CC=$(TARGET_CC) CFLAGS="$(TARGET_CFLAGS) -DIPV6=1" -C $(PKG_BUILD_DIR) clean $(PKG_NAME)
	mv $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_BUILD_DIR)/$(PKG_NAME)6
	touch $@

$(IPKG_FPING):
	mkdir -p $(IDIR_FPING)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME)4 $(IDIR_FPING)/usr/bin/$(PKG_NAME)
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME)6 $(IDIR_FPING)/usr/bin/
	$(STRIP) $(IDIR_FPING)/usr/bin/*
	$(IPKG_BUILD) $(IDIR_FPING) $(PACKAGE_DIR)

