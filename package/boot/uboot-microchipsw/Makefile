include $(TOPDIR)/rules.mk

PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/microchip-ung/u-boot.git
PKG_SOURCE_DATE:=2025-09-22
PKG_SOURCE_VERSION:=c1abab62a00a6b8f43f663a0a30dd0a7fd6a4e95
PKG_MIRROR_HASH:=6405c426afd8bacbb4cc985da4ca1bd04b54a0d2aa3bb3ba8f7571dbe5913055

PKG_MAINTAINER:=Robert Marko <robert.marko@sartura.hr>

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define U-Boot/Default
  BUILD_TARGET:=microchipsw
  HIDDEN:=1
  UBOOT_IMAGE:=u-boot.bin
endef

define U-Boot/ev23x71a
  NAME:=Microchip EV23X71A
  BUILD_DEVICES:=microchip_ev23x71a
  BUILD_SUBTARGET:=lan969x
  UBOOT_CONFIG:=mchp_lan969x
endef

UBOOT_TARGETS:= ev23x71a

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(UBOOT_IMAGE)
endef

$(eval $(call BuildPackage/U-Boot))
