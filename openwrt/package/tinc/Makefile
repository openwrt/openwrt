# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := tinc
PKG_VERSION := 1.0.3
PKG_RELEASE := 1
PKG_MD5SUM := 5dd1acf7d784e29aae78c31a2630fccd

PKG_SOURCE_SITE := http://www.tinc-vpn.org/packages
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_CAT := zcat
PKG_IPK_DIR := $(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE_FILE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE_FILE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.unpacked
	(cd $(PKG_BUILD_DIR) ; \
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
	--datadir=/usr/share \
	--infodir=/usr/share/info \
	--mandir=/usr/share/man \
	--localstatedir=/var \
	--sysconfdir=/etc \
	--with-kernel=$(LINUX_DIR) \
	--with-zlib=$(STAGING_DIR)/usr/ \
	$(DISABLE_NLS) )
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/$(PKG_NAME): $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
	CC=$(TARGET_CC) \
	CFLAGS="$(TARGET_CFLAGS)" \
	all

$(PKG_IPK): $(PKG_BUILD_DIR)/$(PKG_NAME)
	mkdir -p $(PKG_IPK_DIR)/usr/bin
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	cp $(PKG_BUILD_DIR)/src/tincd $(PKG_IPK_DIR)/usr/bin/
	$(STRIP) $(PKG_IPK_DIR)/usr/bin/*
	mkdir -p $(PACKAGE_DIR)
	find $(PKG_IPK_DIR) -name CVS | xargs rm -rf
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/tinc.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.unpacked
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/tinc.list

clean:  
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)
