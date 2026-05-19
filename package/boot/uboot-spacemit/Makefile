# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=uboot-spacemit

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/spacemit-com/uboot-2022.10.git
PKG_SOURCE_DATE:=2026-01-28
PKG_SOURCE_VERSION:=d61c8c77e241314438dce31d9ff1b1cbd9d53688
PKG_MIRROR_HASH:=59bb190629f97c3de683132647a869011ae3a791cf18af496ea855cb581da10f

PKG_RELEASE:=1

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

# build an user installable package
PKG_TARGETS:=
PKG_BUILD_DEPENDS:=opensbi

define U-Boot/spacemit_k1
  NAME:=SpacemiT K1
  BUILD_TARGET:=spacemit
  BUILD_SUBTARGET:=k1
  UBOOT_CONFIG:=k1
  UBOOT_IMAGE:= \
	FSBL.bin \
	u-boot.itb \
	bootinfo_emmc.bin \
	bootinfo_sd.bin \
	bootinfo_spinand.bin \
	bootinfo_spinor.bin
  DEPENDS:=+mtd
endef

UBOOT_TARGETS:= \
	spacemit_k1

UBOOT_MAKE_FLAGS += OPENSBI=$(STAGING_DIR_IMAGE)/fw_dynamic-generic.bin

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)/u-boot-$(BUILD_VARIANT)
	$(INSTALL_DATA) $(patsubst %,$(PKG_BUILD_DIR)/%,$(UBOOT_IMAGE)) $(STAGING_DIR_IMAGE)/u-boot-$(BUILD_VARIANT)/
	$(INSTALL_BIN) bootstrap.sh $(STAGING_DIR_IMAGE)/u-boot-$(BUILD_VARIANT)/
	$(TAR) -C $(STAGING_DIR_IMAGE) -cvzf $(STAGING_DIR_IMAGE)/u-boot-$(BUILD_VARIANT).tar.gz u-boot-$(BUILD_VARIANT)/
	$(INSTALL_DATA) $(STAGING_DIR_IMAGE)/u-boot-$(BUILD_VARIANT).tar.gz $(BIN_DIR)/
endef

define Package/u-boot/install
	$(INSTALL_DIR) $(1)/usr/share/bootloader
	$(INSTALL_DATA) $(patsubst %,$(PKG_BUILD_DIR)/%,$(UBOOT_IMAGE)) $(1)/usr/share/bootloader/
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) update-bootloader.sh $(1)/sbin/
endef

$(eval $(call BuildPackage/U-Boot))
