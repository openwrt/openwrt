include $(TOPDIR)/rules.mk

PKG_VERSION:=2024.04
PKG_HASH:=18a853fe39fad7ad03a90cc2d4275aeaed6da69735defac3492b80508843dd4a
PKG_RELEASE:=$(AUTORELEASE)

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=bmips
endef

define U-Boot/xg6846
  NAME:=Inteno XG6846
  UBOOT_CONFIG:=inteno_xg6846_ram
  BUILD_DEVICES:=inteno_xg6846
  BUILD_TARGET:=bmips
  BUILD_SUBTARGET:=bcm6328
  UBOOT_BOARD:=$(1)
endef

define U-Boot/netgear_cg3100d_ram
  NAME:=Netgear CG3100D
  UBOOT_CONFIG:=netgear_cg3100d_ram
  BUILD_DEVICES:=netgear_cg3100d_ram
  BUILD_TARGET:=bmips
  BUILD_SUBTARGET:=bcm3380
  UBOOT_IMAGE:=u-boot.bin
endef

UBOOT_TARGETS := \
	netgear_cg3100d_ram \
	xg6846

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(UBOOT_IMAGE)
endef

$(eval $(call BuildPackage/U-Boot))
