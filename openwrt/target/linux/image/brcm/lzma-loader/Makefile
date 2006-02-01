# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := loader
PKG_VERSION := 0.04

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

$(PKG_BUILD_DIR)/.prepared:
	mkdir $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	touch $@

$(PKG_BUILD_DIR)/loader.gz: $(PKG_BUILD_DIR)/.prepared
	$(MAKE) -C $(PKG_BUILD_DIR) CC=$(TARGET_CC) \
		LD=$(TARGET_CROSS)ld CROSS_COMPILE=$(TARGET_CROSS)

$(BUILD_DIR)/loader.gz: $(PKG_BUILD_DIR)/loader.gz
	$(CP) $< $@

$(BUILD_DIR)/loader.elf: $(PKG_BUILD_DIR)/loader.o
	$(CP) $< $@

source: 
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(BUILD_DIR)/loader.gz $(BUILD_DIR)/loader.elf
install:

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(BUILD_DIR)/loader.gz
