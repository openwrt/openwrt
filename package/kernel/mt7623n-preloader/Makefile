#
# Copyright (C) 2012-2016 OpenWrt.org
# Copyright (C) 2017 LEDE project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
#include $(INCLUDE_DIR)/image.mk

PKG_NAME:=mt7623n-preloader
PKG_VERSION:=2017-08-03
PKG_RELEASE:=888ad5203d0c9d7dd3d6855c2a9234a70d78dc50

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)/$(PKG_NAME)-$(PKG_RELEASE)

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

BPI_PRELOADER_URL:=@GITHUB/BPI-SINOVOIP/BPI-files/$(PKG_RELEASE)/SD/100MB/
BPI_PRELOADER_PREFIX:=bpi-preloader-$(PKG_RELEASE)

define Download/BPI-R2-preloader-2k.img.gz
  FILE:=$(BPI_PRELOADER_PREFIX)-BPI-R2-preloader-2k.img.gz
  URL:=$(BPI_PRELOADER_URL)
  URL_FILE:=BPI-R2-preloader-2k.img.gz
  HASH:=9a961a26be17dd410866a2385fca47b6f0e7cce88eed6248ff5fb145cf935d93
endef
$(eval $(call Download,BPI-R2-preloader-2k.img.gz))


define Package/mt7623n-preloader
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_mediatek_mt7623
  TITLE:=mt7623n-preloader
  DEFAULT:=y if TARGET_mediatek
endef

define Package/mt7623n-preloader/description
 Preloader image for mt7623n based boards like Bannana Pi R2. 
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	cp $(DL_DIR)/$(BPI_PRELOADER_PREFIX)-BPI-R2-preloader-2k.img.gz $(PKG_BUILD_DIR)/bpi-r2-preloader.bin.gz
endef

define Build/Compile
	true
endef

define Package/brcm2708-gpu-fw/install
	true
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR_IMAGE)
	gunzip -c $(PKG_BUILD_DIR)/bpi-r2-preloader.bin.gz > $(STAGING_DIR_IMAGE)/bpi-r2-preloader.bin
endef

$(eval $(call BuildPackage,mt7623n-preloader))
