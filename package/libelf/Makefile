# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := libelf
PKG_VERSION := 0.8.5
PKG_RELEASE := 1
PKG_MD5SUM := c1daf069367871350ece779b7de20047

PKG_SOURCE_SITE := http://www.mr511.de/software/
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR :=$(PKG_BUILD_DIR)/ipkg/

.NOTPARALLEL:

$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.unpacked
	(cd $(PKG_BUILD_DIR); \
	echo "mr_cv_target_elf=$${mr_cv_target_elf=yes}" > config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/ \
		--sysconfdir=/etc \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/lib/libelf.so.$(LIBELF_VER): $(PKG_BUILD_DIR)/.configured
	$(MAKE) -j1 CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld ARCH=$(ARCH) \
		-C $(PKG_BUILD_DIR)

$(STAGING_DIR)/lib/libelf.so: $(PKG_BUILD_DIR)/lib/libelf.so.$(LIBELF_VER)
	$(MAKE) -j1 CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld ARCH=$(ARCH) \
		-C $(PKG_BUILD_DIR)/lib instroot="$(STAGING_DIR)" install


$(PKG_IPK): $(PKG_BUILD_DIR)/lib/libelf.so.$(LIBELF_VER)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/lib
	cp -a $(PKG_BUILD_DIR)/lib/libelf.so* $(PKG_IPK_DIR)/usr/lib/
	$(STRIP) $(PKG_IPK_DIR)/usr/lib/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME).list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/.unpacked
compile: $(STAGING_DIR)/lib/libelf.so $(PKG_IPK)
install: $(PKG_ZLIB) $(IPKG_STATE_DIR)/info/$(PKG_NAME).list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(STAGING_DIR)/lib/$(PKG_NAME)* 
	rm -f $(PACKAGE_DIR)/$(PKG_NAME)\*.ipk
