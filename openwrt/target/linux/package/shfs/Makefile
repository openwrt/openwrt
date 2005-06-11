# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := shfs
PKG_VERSION := 0.35
PKG_RELEASE := 2
PKG_MD5SUM := 016f49d71bc32eee2b5d11fc1600cfbe

PKG_SOURCE_URL := @SF/shfs
PKG_SOURCE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT := zcat

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

ifeq ($(KERNEL_DIR),)
KERNEL_DIR:=$(LINUX_DIR)
endif

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_SHFS,kmod-shfs,$(LINUX_VERSION)$(BOARD)+$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(KERNEL_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: 
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		OFLAGS="$(TARGET_CFLAGS)" \
		CC="$(TARGET_CC)" \
		LINKER="$(TARGET_CC)" \
		KERNEL="$(LINUX_VERSION)" \
		KERNEL_SOURCES="$(KERNEL_DIR)" \
		ROOT="$(PKG_INSTALL_DIR)" \
		module module-install
	touch $@

$(IPKG_KMOD_SHFS):
	install -m0755 -d $(IDIR_KMOD_SHFS)/lib/modules/$(LINUX_VERSION)
	cp -fpR $(PKG_INSTALL_DIR)/lib/modules/$(LINUX_VERSION)/kernel/fs/shfs/shfs.o \
	 $(IDIR_KMOD_SHFS)/lib/modules/$(LINUX_VERSION)/
	$(RSTRIP_KMOD) $(IDIR_KMOD_SHFS)
	$(IPKG_BUILD) $(IDIR_KMOD_SHFS) $(PACKAGE_DIR)

