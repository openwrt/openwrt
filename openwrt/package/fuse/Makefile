# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := fuse
PKG_VERSION := 2.2.1
PKG_RELEASE := 1
PKG_MD5SUM := 250d89b9c7b6ecf531df60c67f75737d

PKG_SOURCE_SITE := @SF/fuse
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)

I_KMOD_FUSE := $(PACKAGE_DIR)/kmod-fuse_$(LINUX_VERSION)+$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
I_LIBFUSE := $(PACKAGE_DIR)/libfuse_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
I_FUSE_UTILS := $(PACKAGE_DIR)/fuse-utils_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk


$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/ipkg/rules: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	rm -rf $(PKG_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	cp -fpR ./ipkg $(PKG_BUILD_DIR)/
	find $(PKG_BUILD_DIR) -name CVS | xargs rm -rf
	chmod a+x $(PKG_BUILD_DIR)/ipkg/rules
	touch $(PKG_BUILD_DIR)/ipkg/rules

$(I_KMOD_FUSE) $(I_LIBFUSE) $(I_FUSE_UTILS): $(PKG_BUILD_DIR)/ipkg/rules
	cd $(PKG_BUILD_DIR); \
	TOPDIR="$(TOPDIR)" IPKG_RULES_INC="$(TOPDIR)/rules.mk" \
	./ipkg/rules package

$(IPKG_STATE_DIR)/info/kmod-fuse.list: $(I_KMOD_FUSE)
	$(IPKG) install $(I_KMOD_FUSE)

$(IPKG_STATE_DIR)/info/libfuse.list: $(I_LIBFUSE)
	$(IPKG) install $(I_LIBFUSE)

$(IPKG_STATE_DIR)/info/fuse-utils.list: $(I_FUSE_UTILS)
	$(IPKG) install $(I_FUSE_UTILS)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/ipkg/rules
compile: $(I_KMOD_FUSE) $(I_FUSE_UTILS)
install: $(IPKG_STATE_DIR)/info/kmod-fuse.list $(IPKG_STATE_DIR)/info/libfuse.list $(IPKG_STATE_DIR)/info/fuse-utils.list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(I_KMOD_FUSE) $(I_LIBFUSE) $(I_FUSE_UTILS)

