include $(TOPDIR)/rules.mk

PKG_NAME:=cortina-ucode
PKG_VERSION:=21.08
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/NXP/qoriq-firmware-cortina.git
PKG_SOURCE_VERSION:=LSDK-21.08
PKG_MIRROR_HASH:=c0ba3953000ab8633d9efede32bddf5b34c6f91c4ef078b9a0df41d903fa7af2

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/qoriq-cortina
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=NXP Cortina CS4315/CS4340 ucode
  DEPENDS:=@TARGET_layerscape||TARGET_qoriq
endef

define Build/Compile
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/cs4315-cs4340-PHY-ucode.txt \
		$(STAGING_DIR_IMAGE)/cs4315-cs4340-PHY-ucode.txt
endef

$(eval $(call BuildPackage,qoriq-cortina))
