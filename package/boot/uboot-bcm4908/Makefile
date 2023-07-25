# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.openwrt.org/project/bcm63xx/u-boot.git
PKG_SOURCE_DATE:=2022-12-08
PKG_SOURCE_VERSION:=4435700d18a791dca0d8d767e5414dfac9df4451
PKG_MIRROR_HASH:=6062ce611d7222eb3b9768bb4944ff1c7bcf26b997280adf5ea8d7afe83f28a8

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=bcm4908
  UBOOT_IMAGE:=u-boot-nodtb.bin
  DEFAULT:=y
endef

define U-Boot/bcm4908
  NAME:=Broadcom's BCM4908
  UBOOT_CONFIG:=bcm94908
  SOC:=bcm4908
endef

define U-Boot/bcm4912
  NAME:=Broadcom's BCM4912
  UBOOT_CONFIG:=bcm94912
  SOC:=bcm4912
endef

UBOOT_TARGETS := \
	bcm4908 \
	bcm4912

define Build/Prepare
	$(call Build/Prepare/Default)
	mkdir -p $(PKG_BUILD_DIR)/include/generated/
	( cd $(PKG_BUILD_DIR)/board/broadcom/bcmbca/httpd/html/ && \
	  $(STAGING_DIR_HOST)/bin/xxd -i index.html > ../../../../../include/generated/index.h && \
	  $(STAGING_DIR_HOST)/bin/xxd -i flashing.html > ../../../../../include/generated/flashing.h && \
	  $(STAGING_DIR_HOST)/bin/xxd -i fail.html > ../../../../../include/generated/fail.h && \
	  $(STAGING_DIR_HOST)/bin/xxd -i 404.html > ../../../../../include/generated/404.h )
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)/u-boot
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/u-boot/u-boot-$(SOC).bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/u-boot.dtb $(STAGING_DIR_IMAGE)/u-boot/u-boot-$(SOC).dtb
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/arch/arm/dts/*.dtb $(STAGING_DIR_IMAGE)/u-boot/
endef

$(eval $(call BuildPackage/U-Boot))
