include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2024.10
PKG_HASH:=b28daf4ac17e43156363078bf510297584137f6df50fced9b12df34f61a92fb0

UBOOT_USE_INTREE_DTC:=1

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define U-Boot/Default
  BUILD_TARGET:=ath79
  BUILD_SUBTARGET:=generic
  UBOOT_IMAGE:=u-boot.bin
  UBOOT_CONFIG:=ap121
  HIDDEN:=1
endef

define U-Boot/ar9344_nec_aterm
  NAME:=NEC Aterm series (AR9344)
  BUILD_SUBTARGET:= tiny
  BUILD_DEVICES:=nec_wg600hp nec_wr8750n nec_wr9500n
  UBOOT_CONFIG:=nec_ar9344_aterm
endef

define U-Boot/qca9558_nec_aterm
  NAME:=NEC Aterm series (QCA9558)
  BUILD_SUBTARGET:= generic
  BUILD_DEVICES:=nec_wg1400hp nec_wg1800hp nec_wg1800hp2
  UBOOT_CONFIG:=nec_qca9558_aterm
endef

UBOOT_TARGETS := ar9344_nec_aterm qca9558_nec_aterm

# don't stage files to bindir, let target/linux/ath79/image/*.mk do that
define Package/u-boot/install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(UBOOT_IMAGE)
endef

$(eval $(call BuildPackage/U-Boot))
