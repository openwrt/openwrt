include $(TOPDIR)/rules.mk

PKG_RELEASE:=1
PKG_SOURCE_PROTO:=git
PKG_VERSION:=2026.10-rc5
PKG_SOURCE_URL:=https://git.u-boot-project.org/u-boot/u-boot.git
PKG_SOURCE_VERSION:=a06e89ab05eaa2b8344d521319399333cd760ae5
PKG_MIRROR_HASH:=26bd795d7b727d9c71bbf6cfd7d3d1b1465ec706a2a3be2e529a70f396283c6a

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
