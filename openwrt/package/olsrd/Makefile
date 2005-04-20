# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=olsrd
PKG_VERSION:=0.4.9
PKG_RELEASE:=1
PKG_MD5SUM:=593c0861fa10d2a8d0e7d8617479c5cf

PKG_SOURCE_URL:=http://www.olsr.org/releases/0.4
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=bzcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/$(PKG_NAME): $(PKG_BUILD_DIR)/.unpacked
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		OS="linux" \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -DOLSR_PLUGIN -I. -Isrc -Dlinux -I../../src -DNODEBUG" \
		libs
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		OS="linux" \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -I. -Isrc -Dlinux -I../../src -DNODEBUG" \
		all

$(PKG_IPK): $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/sbin
	mkdir -p $(PKG_IPK_DIR)/usr/lib
	cp $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_IPK_DIR)/usr/sbin/
	cp $(PKG_BUILD_DIR)/lib/dyn_gw/olsrd_dyn_gw.so.0.3 $(PKG_IPK_DIR)/usr/lib/
	cp $(PKG_BUILD_DIR)/lib/nameservice/olsrd_nameservice.so.0.2 $(PKG_IPK_DIR)/usr/lib/
	cp $(PKG_BUILD_DIR)/lib/httpinfo/olsrd_httpinfo.so.0.1 $(PKG_IPK_DIR)/usr/lib/
	cp $(PKG_BUILD_DIR)/lib/secure/olsrd_secure.so.0.5 $(PKG_IPK_DIR)/usr/lib/
	$(STRIP) $(PKG_IPK_DIR)/usr/sbin/*
	$(STRIP) $(PKG_IPK_DIR)/usr/lib/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME).list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.unpacked
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/$(PKG_NAME).list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)
