# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := shfs
PKG_VERSION := 0.35
PKG_RELEASE := 2
PKG_MD5SUM := 016f49d71bc32eee2b5d11fc1600cfbe

PKG_SOURCE_URL := \
	@SF/shfs \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT := zcat
PKG_SOURCE_DIR := 

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/templates.mk

$(eval $(call PKG_template,KMOD_SHFS,kmod-shfs,$(LINUX_VERSION)+$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SHFS_UTILS,shfs-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

include $(TOPDIR)/package/rules.mk

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.prepared
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  OFLAGS="$(TARGET_CFLAGS)" \
	  CC="$(TARGET_CC)" \
	  LINKER="$(TARGET_CC)" \
	  KERNEL="$(LINUX_VERSION)" \
	  KERNEL_SOURCES="$(LINUX_DIR)" \
	 all
	touch $(PKG_BUILD_DIR)/.built

$(PKG_BUILD_DIR)/.installed: $(PKG_BUILD_DIR)/.built
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  ROOT="$(PKG_INSTALL_DIR)" \
	  KERNEL="$(LINUX_VERSION)" \
	  KERNEL_SOURCES="$(LINUX_DIR)" \
	 install
	touch $(PKG_BUILD_DIR)/.installed

$(IPKG_KMOD_SHFS): $(IDIR_KMOD_SHFS)/CONTROL/control $(PKG_BUILD_DIR)/.installed
	install -m0755 -d $(IDIR_KMOD_SHFS)/lib/modules/$(LINUX_VERSION)
	cp -fpR $(PKG_INSTALL_DIR)/lib/modules/$(LINUX_VERSION)/kernel/fs/shfs/shfs.o \
	 $(IDIR_KMOD_SHFS)/lib/modules/$(LINUX_VERSION)/
	$(RSTRIP_KMOD) $(IDIR_KMOD_SHFS)
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_KMOD_SHFS) $(PACKAGE_DIR)

$(IPKG_SHFS_UTILS): $(IDIR_SHFS_UTILS)/CONTROL/control $(PKG_BUILD_DIR)/.installed
	install -m0755 -d $(IDIR_SHFS_UTILS)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/shfs{,u}mount $(IDIR_SHFS_UTILS)/usr/sbin/
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/mount.shfs $(IDIR_SHFS_UTILS)/usr/sbin/
	$(RSTRIP) $(IDIR_SHFS_UTILS)
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_SHFS_UTILS) $(PACKAGE_DIR)

