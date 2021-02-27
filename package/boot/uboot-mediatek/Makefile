include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2020.10
PKG_HASH:=0d481bbdc05c0ee74908ec2f56a6daa53166cc6a78a0e4fac2ac5d025770a622
PKG_BUILD_DEPENDS:=arm-trusted-firmware-tools/host

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define U-Boot/Default
  BUILD_TARGET:=mediatek
  UBOOT_IMAGE:=u-boot-mtk.bin
  DEFAULT:=y
endef

define U-Boot/mt7622_rfb1
  NAME:=MT7622 Reference Board 1
  BUILD_SUBTARGET:=mt7622
  UBOOT_CONFIG:=mt7622_rfb
endef

define U-Boot/mt7622_linksys_e8450
  NAME:=Linksys E8450
  BUILD_SUBTARGET:=mt7622
  UBOOT_CONFIG:=mt7622_linksys_e8450
  UBOOT_IMAGE:=u-boot.fip
  BL2_BOOTDEV:=snand
  BL2_DDRBLOB:=1
  DEPENDS:=+trusted-firmware-a-mt7622-snand-1ddr
endef

define U-Boot/mt7622_bananapi_bpi-r64-emmc
  NAME:=BananaPi R64 (eMMC)
  BUILD_SUBTARGET:=mt7622
  UBOOT_CONFIG:=mt7622_bananapi_bpi-r64-emmc
  UBOOT_IMAGE:=u-boot.fip
  BL2_BOOTDEV:=emmc
  BL2_DDRBLOB:=2
  DEPENDS:=+trusted-firmware-a-mt7622-emmc-2ddr
endef

define U-Boot/mt7622_bananapi_bpi-r64-sdmmc
  NAME:=BananaPi R64 (SDMMC)
  BUILD_SUBTARGET:=mt7622
  UBOOT_CONFIG:=mt7622_bananapi_bpi-r64-sdmmc
  UBOOT_IMAGE:=u-boot.fip
  BL2_BOOTDEV:=sdmmc
  BL2_DDRBLOB:=2
  DEPENDS:=+trusted-firmware-a-mt7622-sdmmc-2ddr
endef

define U-Boot/mt7623a_unielec_u7623
  NAME:=UniElec U7623 (mt7623)
  BUILD_SUBTARGET:=mt7623
  UBOOT_CONFIG:=mt7623a_unielec_u7623_02
endef

define U-Boot/mt7623n_bpir2
  NAME:=Banana Pi R2 (mt7623)
  BUILD_SUBTARGET:=mt7623
  UBOOT_IMAGE:=u-boot.bin
  UBOOT_CONFIG:=mt7623n_bpir2
endef

define U-Boot/mt7629
  NAME:=MT7629
  BUILD_SUBTARGET:=mt7629
  UBOOT_CONFIG:=mt7629_rfb
endef

UBOOT_TARGETS := \
	mt7629 \
	mt7622_bananapi_bpi-r64-emmc \
	mt7622_bananapi_bpi-r64-sdmmc \
	mt7622_linksys_e8450 \
	mt7622_rfb1 \
	mt7623n_bpir2 \
	mt7623a_unielec_u7623

UBOOT_MAKE_FLAGS += $(UBOOT_IMAGE:.fip=.bin)

Build/Exports:=$(Host/Exports)

define Build/fip-image
	$(STAGING_DIR_HOST)/bin/fiptool create \
		--soc-fw $(STAGING_DIR_IMAGE)/$(BUILD_SUBTARGET)-$(BL2_BOOTDEV)-$(BL2_DDRBLOB)ddr-bl31.bin \
		--nt-fw $(PKG_BUILD_DIR)/u-boot.bin \
		$(PKG_BUILD_DIR)/u-boot.fip
endef

define Build/Compile
	$(call Build/Compile/U-Boot)
ifeq ($(UBOOT_IMAGE),u-boot.fip))
	$(call Build/fip-image)
endif
endef

# don't stage files to bindir, let target/linux/mediatek/image/*.mk do that
define Package/u-boot/install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(UBOOT_IMAGE)
endef

$(eval $(call BuildPackage/U-Boot))
