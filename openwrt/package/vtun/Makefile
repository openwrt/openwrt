# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := vtun
PKG_VERSION := 2.6
PKG_RELEASE := 1
PKG_MD5SUM := 309534fd03c5d13a19c43916f61f4bbf

PKG_SOURCE_URL := @SF/vtun
PKG_SOURCE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
PKG_CAT := zcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,VTUN,vtun,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR) ; \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS) \
	-I$(STAGING_DIR)/usr/include" \
	LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
	./configure \
	--target=$(GNU_TARGET_NAME) \
	--host=$(GNU_TARGET_NAME) \
	--build=$(GNU_HOST_NAME) \
	--prefix=/usr \
	--exec-prefix=/usr \
	--bindir=/usr/bin \
	--sbindir=/usr/sbin \
	--libexecdir=/usr/lib \
	--datadir=/usr/share \
	--infodir=/usr/share/info \
	--mandir=/usr/share/man \
	--localstatedir=/var \
	--sysconfdir=/etc \
	--with-kernel=$(LINUX_DIR) \
	--with-zlib=$(STAGING_DIR)/usr/ \
	--with-lzo-headers=$(STAGING_DIR)/usr/include \
	--with-lzo-libs=$(STAGING_DIR)/usr/lib \
	$(DISABLE_NLS) )
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
	CC=$(TARGET_CC) \
	CFLAGS="$(TARGET_CFLAGS) \
	-I$(STAGING_DIR)/usr/include" \
	LDFLAGS="-L$(STAGING_DIR)/usr/lib -lssl -lz -llzo -lcrypto" \
	vtund
	touch $@

$(IPKG_VTUN):
	mkdir -p $(IDIR_VTUN)/usr/sbin
	cp $(PKG_BUILD_DIR)/vtund $(IDIR_VTUN)/usr/sbin/
	$(STRIP) $(IDIR_VTUN)/usr/sbin/*
	mkdir -p $(IDIR_VTUN)/etc
	cp $(PKG_BUILD_DIR)/vtund.conf $(IDIR_VTUN)/etc
	$(IPKG_BUILD) $(IDIR_VTUN) $(PACKAGE_DIR)
