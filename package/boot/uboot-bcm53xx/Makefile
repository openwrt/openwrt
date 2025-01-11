include $(TOPDIR)/rules.mk

PKG_VERSION:=2025.01
PKG_HASH:=cdef7d507c93f1bbd9f015ea9bc21fa074268481405501945abc6f854d5b686f
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
