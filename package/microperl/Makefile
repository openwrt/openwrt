# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=microperl
PKG_VERSION:=5.8.6
PKG_RELEASE:=1
PKG_MD5SUM:=8d4aa4e061fd5bc0c39e4f0a63267880

PKG_SOURCE_URL:=ftp://ftp.cpan.org/pub/CPAN/src/5.0
PKG_SOURCE:=perl-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/perl-$(PKG_VERSION)
PKG_CAT:=zcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE):
	 $(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/$(PKG_NAME): $(PKG_BUILD_DIR)/.unpacked
	$(MAKE) -f Makefile.micro CC="$(TARGET_CC)" OPTIMIZE="$(TARGET_CFLAGS)" -C $(PKG_BUILD_DIR)

$(PKG_IPK): $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/bin
	cp $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_IPK_DIR)/usr/bin/
	$(STRIP) $(PKG_IPK_DIR)/usr/bin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.unpacked
compile: $(PKG_IPK)
install:
	$(IPKG) install $(PKG_IPK)
clean:
	rm -rf $(PKG_BUILD_DIR)
