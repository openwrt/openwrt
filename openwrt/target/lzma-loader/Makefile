# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := loader
PKG_VERSION := 0.04
PKG_MD5SUM := c0b7c36232d3910c425d03e56d0f532b

PKG_SOURCE_SITE := http://wl500g.dyndns.org/loader/
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)

$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/loader.gz: $(PKG_BUILD_DIR)/.patched
	$(MAKE) -C $(PKG_BUILD_DIR) CC=$(TARGET_CC) \
		LD=$(TARGET_CROSS)ld CROSS_COMPILE=$(TARGET_CROSS)

$(BUILD_DIR)/loader.gz: $(PKG_BUILD_DIR)/loader.gz
	cp $< $@

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(BUILD_DIR)/loader.gz
install:

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(BUILD_DIR)/loader.gz
