# ipkg-utils
#
# http://handhelds.org/packages/ipkg-utils
#

PKG_NAME := ipkg-utils
PKG_VERSION := 1.7

PKG_SOURCE_SITE := http://handhelds.org/packages/ipkg-utils/
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/rules.mk

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)


$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) x $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/.patched: $(PKG_BUILD_DIR)/.unpacked
	$(PATCH) $(PKG_BUILD_DIR) $(PKG_VERSION)/ \*.patch
	touch $(PKG_BUILD_DIR)/.patched

$(STAGING_DIR)/usr/bin/ipkg-build: $(PKG_BUILD_DIR)/.patched
	mkdir -p $(STAGING_DIR)/etc
	echo "dest root /" > $(STAGING_DIR)/etc/ipkg.conf
	echo "option offline_root $(TARGET_DIR)" >> $(STAGING_DIR)/etc/ipkg.conf
	mkdir -p $(STAGING_DIR)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/ipkg-build $(STAGING_DIR)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/ipkg-buildpackage $(STAGING_DIR)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/ipkg-make-index $(STAGING_DIR)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/ipkg.py $(STAGING_DIR)/usr/bin

source: $(DL_DIR)/$(PKG_SOURCE_FILE)

prepare: $(PKG_BUILD_DIR)/.patched

compile:

install: $(STAGING_DIR)/usr/bin/ipkg-build

clean:
	rm -f $(STAGING_DIR)/etc/ipkg.conf
	rm -f $(STAGING_DIR)/usr/bin/ipkg*
	rm -rf $(PKG_BUILD_DIR)


