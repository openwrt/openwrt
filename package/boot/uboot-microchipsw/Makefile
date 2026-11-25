include $(TOPDIR)/rules.mk

PKG_RELEASE:=1
PKG_VERSION:=2026.10
PKG_HASH:=3f856da3cbbc93af51ff95fde18588a348b4a26723b01d9841e999d988481015

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
  UBOOT_CONFIG:=microchip_ev23x71a
endef

define U-Boot/tactical-1000
  NAME:=Novarq Tactical 1000
  BUILD_DEVICES:=novarq_tactical-1000
  BUILD_SUBTARGET:=lan969x
  UBOOT_CONFIG:=novarq_tactical_1000
endef

UBOOT_TARGETS:= \
	ev23x71a \
	tactical-1000

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(UBOOT_IMAGE)
endef

$(eval $(call BuildPackage/U-Boot))
