# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := shfs
PKG_VERSION := 0.35
PKG_RELEASE := 2
PKG_MD5SUM := 016f49d71bc32eee2b5d11fc1600cfbe

PKG_SOURCE_SITE := @SF/shfs
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)

I_KMOD_SHFS := $(PACKAGE_DIR)/kmod-shfs_$(LINUX_VERSION)+$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
I_SHFS_UTILS := $(PACKAGE_DIR)/shfs-utils_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk


$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/ipkg/rules: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	rm -rf $(PKG_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	cp -fpR ./ipkg $(PKG_BUILD_DIR)/
	chmod a+x $(PKG_BUILD_DIR)/ipkg/rules
	touch $(PKG_BUILD_DIR)/ipkg/rules

$(I_KMOD_SHFS) $(I_SHFS_UTILS): $(PKG_BUILD_DIR)/ipkg/rules
	cd $(PKG_BUILD_DIR); \
	TOPDIR="$(TOPDIR)" IPKG_RULES_INC="$(TOPDIR)/rules.mk" \
	./ipkg/rules package

$(IPKG_STATE_DIR)/info/kmod-shfs.list: $(I_KMOD_SHFS)
	$(IPKG) install $(I_KMOD_SHFS)

$(IPKG_STATE_DIR)/info/shfs-utils.list: $(I_SHFS_UTILS)
	$(IPKG) install $(I_SHFS_UTILS)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/ipkg/rules
compile: $(I_KMOD_SHFS) $(I_SHFS_UTILS)
install: $(IPKG_STATE_DIR)/info/kmod-shfs.list $(IPKG_STATE_DIR)/info/shfs-utils.list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(I_KMOD_SHFS) $(I_SHFS_UTILS)

