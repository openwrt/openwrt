# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=bridge
PKG_VERSION:=1.0.6
PKG_RELEASE:=1
PKG_MD5SUM:=9b7dc52656f5cbec846a7ba3299f73bd

PKG_SOURCE_URL:=@SF/bridge
PKG_SOURCE:=bridge-utils-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/bridge-utils-$(PKG_VERSION)
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk
$(eval $(call PKG_template,BRIDGE,bridge,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
		--with-linux-headers=$(LINUX_DIR) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)
	touch $@

$(IPKG_BRIDGE): 
	mkdir -p $(IDIR_BRIDGE)/usr/sbin
	cp $(PKG_BUILD_DIR)/brctl/brctl $(IDIR_BRIDGE)/usr/sbin/
	$(STRIP) $(IDIR_BRIDGE)/usr/sbin/brctl
	$(IPKG_BUILD) $(IDIR_BRIDGE) $(PACKAGE_DIR)
