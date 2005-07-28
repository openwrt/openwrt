# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := base-files-arch
PKG_RELEASE := 1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

IDIR_OPENWRT:=$(PKG_BUILD_DIR)/ipkg
IPKG_OPENWRT:=$(PACKAGE_DIR)/base-files-$(BOARD)_$(PKG_RELEASE)_$(ARCH).ipk

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $(PKG_BUILD_DIR)
	touch $@

ifeq ($(BOARD),brcm)
$(PKG_BUILD_DIR)/jffs2root: jffs2root.c
	$(TARGET_CC) -o $@ $<

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/jffs2root

$(IDIR_OPENWRT)/sbin/jffs2root: $(PKG_BUILD_DIR)/jffs2root
	mkdir -p $(IDIR_OPENWRT)/sbin
	cp $(PKG_BUILD_DIR)/jffs2root $(IDIR_OPENWRT)/sbin

$(IPKG_OPENWRT): $(IDIR_OPENWRT)/sbin/jffs2root
endif

$(PKG_BUILD_DIR)/.built:
	touch $@
	
$(IDIR_OPENWRT):
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_OPENWRT) ipkg/base-files-arch.control $(PKG_RELEASE) $(ARCH)
	$(SED) s,base-files-arch,base-files-$(BOARD),g $(IDIR_OPENWRT)/CONTROL/control

$(IPKG_OPENWRT): $(IDIR_OPENWRT)
	mkdir -p $(IDIR_OPENWRT)/etc
	cp files/network.overrides.$(BOARD) $(IDIR_OPENWRT)/etc/network.overrides
	$(RSTRIP) $(IDIR_OPENWRT)
	$(IPKG_BUILD) $(IDIR_OPENWRT) $(PACKAGE_DIR)

install: $(IPKG_OPENWRT)
	$(IPKG) install $(IPKG_OPENWRT)
compile: install-dev $(IPKG_OPENWRT)
install-dev:
	mkdir -p $(STAGING_DIR)/usr/include
	cp -a ./include/* $(STAGING_DIR)/usr/include/
