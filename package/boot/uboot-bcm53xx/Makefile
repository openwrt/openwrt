include $(TOPDIR)/rules.mk

PKG_VERSION:=2023.04
PKG_HASH:=e31cac91545ff41b71cec5d8c22afd695645cd6e2a442ccdacacd60534069341
PKG_RELEASE:=$(AUTORELEASE)

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=bcm53xx
  BUILD_SUBTARGET:=generic
  UBOOT_CONFIG:=bcmns
  UBOOT_BOARD:=$(1)
endef

define U-Boot/dir-885l
  NAME:=D-Link DIR-885L
  BUILD_DEVICES:=dlink_dir-885l
endef

define U-Boot/dir-890l
  NAME:=D-Link DIR-890L
  BUILD_DEVICES:=dlink_dir-890l
endef

UBOOT_TARGETS := dir-885l dir-890l

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_DEVICES)-u-boot.bin
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
