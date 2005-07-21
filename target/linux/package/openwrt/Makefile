# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := base-files-arch
PKG_RELEASE := 1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

BR2_PACKAGE_OPENWRT:=y		# ignore menuconfig
$(eval $(call PKG_template,OPENWRT,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $(PKG_BUILD_DIR)
	touch $@

ifeq ($(BOARD),brcm)
$(PKG_BUILD_DIR)/jffs2root: jffs2root.c
	$(TARGET_CC) -o $@ $<

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/jffs2root

$(IDIR_OPENWRT)/sbin/jffs2root:
	mkdir -p $(IDIR_OPENWRT)/sbin
	cp $(PKG_BUILD_DIR)/jffs2root $(IDIR_OPENWRT)/sbin

$(IPKG_OPENWRT): $(IDIR_OPENWRT)/sbin/jffs2root
endif

$(PKG_BUILD_DIR)/.built:
	touch $@

$(IPKG_OPENWRT): 
	mkdir -p $(IDIR_OPENWRT)/etc
	cp files/network.overrides.$(BOARD) $(IDIR_OPENWRT)/etc/network.overrides
	$(RSTRIP) $(IDIR_OPENWRT)
	$(IPKG_BUILD) $(IDIR_OPENWRT) $(PACKAGE_DIR)

compile: install-dev
install-dev:
	mkdir -p $(STAGING_DIR)/usr/include
	cp -a ./include/* $(STAGING_DIR)/usr/include/
