# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.openwrt.org/project/bcm63xx/u-boot.git
PKG_SOURCE_DATE:=2022-03-15
PKG_SOURCE_VERSION:=0625aad74d1f5b6f9c068955ad3fd7f6df635e50
PKG_MIRROR_HASH:=0602e0e4f101ead206940eccca832b75191905c1e81290340a89b07dbee7a6ce

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
