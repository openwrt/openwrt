include $(TOPDIR)/rules.mk

PKG_VERSION:=2024.04
PKG_HASH:=d6b57ce574a0a0504a5b6596644ceacb7f77bde9353779bcf2fde07c4b9a2b92
PKG_RELEASE:=2

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=bmips
  BUILD_SUBTARGET:=bcm6328
  UBOOT_CONFIG:=inteno_xg6846_ram
  UBOOT_BOARD:=$(1)
endef

define U-Boot/xg6846
  NAME:=Inteno XG6846
  BUILD_DEVICES:=inteno_xg6846
endef

UBOOT_TARGETS := xg6846

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_DEVICES)-u-boot.bin
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
