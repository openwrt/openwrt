#
# Copyright (C) 2016 Gary Wang <gary.wang@bananapi.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
PKG_NAME:=uboot-mtk-bpi-r2
PKG_VERSION:=v1.0
PKG_RELEASE:=1
HOSTCPPFLAGS:="-I."
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/BPI-SINOVOIP/BPI-R2-LEDE-Uboot.git
PKG_SOURCE_VERSION:=e4ca15169a1f4063a05b3cfaf770b3faee4a3cb5
PKG_LICENSE:=GPL-2.0 GPL-2.0+
PKG_LICENSE_FILES:=Licenses/README
PKG_MAINTAINER:=Gary Wang <gary.wang@bananapi.com>
include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
define U-Boot/Default
  BUILD_TARGET:=mediatek
  UBOOT_IMAGE:=u-boot.bin 
  UENV:=default
  HIDDEN:=1
endef
define U-Boot/bpi_r2
  NAME:=Mediatek Banana Pi R2 board
  BUILD_DEVICES:=bpi_r2
endef
UBOOT_TARGETS := \
	bpi_r2
UBOOT_CONFIGURE_VARS += USE_PRIVATE_LIBGCC=yes
define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/mtk-bpi-r2-uboot.bin
	$(CP) $(PKG_BUILD_DIR)/mt7623_evb_preloader/EMMC_PRELOADER_1600MHz.img $(STAGING_DIR_IMAGE)/mtk-bpi-r2-preloader-emmc.bin
	$(CP) $(PKG_BUILD_DIR)/mt7623_evb_preloader/SD_PRELOADER_1600MHz.img $(STAGING_DIR_IMAGE)/mtk-bpi-r2-preloader-sd.bin
endef
define Package/u-boot/install/default
endef
$(eval $(call BuildPackage/U-Boot))
