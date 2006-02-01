# $Id$

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME:=shfs
PKG_VERSION:=0.35
PKG_RELEASE:=2
PKG_MD5SUM:=016f49d71bc32eee2b5d11fc1600cfbe

PKG_SOURCE_URL:=@SF/shfs
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_SHFS,kmod-shfs,$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: 
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		PATH="$(TARGET_PATH)" \
		OFLAGS="$(TARGET_CFLAGS)" \
		CC="$(TARGET_CC)" \
		LINKER="$(TARGET_CC)" \
		KERNEL="$(LINUX_VERSION)" \
		KERNEL_SOURCES="$(LINUX_DIR)" \
		ROOT="$(PKG_INSTALL_DIR)" \
		module module-install
	touch $@

$(IPKG_KMOD_SHFS):
	install -m0755 -d $(IDIR_KMOD_SHFS)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_INSTALL_DIR)/lib/modules/$(LINUX_VERSION)/kernel/fs/shfs/shfs.$(LINUX_KMOD_SUFFIX) \
	 $(IDIR_KMOD_SHFS)/lib/modules/$(LINUX_VERSION)/
	$(IPKG_BUILD) $(IDIR_KMOD_SHFS) $(PACKAGE_DIR)

