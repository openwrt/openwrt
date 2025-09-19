include $(TOPDIR)/rules.mk

PKG_NAME:=extroot
PKG_RELEASE:=1
PKG_VERSION:=1.0
PKG_FLAGS+=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/extroot
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=External (e.g. USB) rootfs support
  DEPENDS:=+sfdisk +e2fsprogs
endef

define Package/extroot/description
  Allow the root filesystem overlay to be stored on an external block device such as USB.
  
  When the OS boots without any onboard writable storage partition (e.g. JFFS or UBI),
  it will attempt to partition any qualifying block device it finds. A block device
  qualifies if it has been overwritten with the exact bytes:
  'OPENWRT_ROOT_DELETE_ALL_DATA'. After partitioning, the OS will automatically load the
  disk on boot.

  For this to work, you must include the required filesystem and device drivers as =y and
  you probably need to set BUILTIN_KMODS so that they will be loaded early enough.
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/extroot/install
	$(INSTALL_DIR) $(1)/lib/preinit
	$(INSTALL_BIN) ./files/etc/preinit.d/75_extroot $(1)/lib/preinit/

	$(INSTALL_DIR) $(1)/etc/hotplug.d/block
	$(INSTALL_BIN) ./files/etc/hotplug.d/block/10-extroot $(1)/etc/hotplug.d/block/
endef

$(eval $(call BuildPackage,extroot))
