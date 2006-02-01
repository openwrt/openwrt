# $Id$

include $(TOPDIR)/rules.mk

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

$(eval $(call PKG_template,SHFS_UTILS,shfs-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: 
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		OFLAGS="$(TARGET_CFLAGS)" \
		CC="$(TARGET_CC)" \
		LINKER="$(TARGET_CC)" \
		KERNEL_SOURCES="$(LINUX_DIR)" \
		ROOT="$(PKG_INSTALL_DIR)" \
		utils utils-install
	touch $@

$(IPKG_SHFS_UTILS):
	install -m0755 -d $(IDIR_SHFS_UTILS)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/shfs{,u}mount $(IDIR_SHFS_UTILS)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/mount.shfs $(IDIR_SHFS_UTILS)/usr/sbin/
	$(RSTRIP) $(IDIR_SHFS_UTILS)
	$(IPKG_BUILD) $(IDIR_SHFS_UTILS) $(PACKAGE_DIR)

