# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=lqtapi
PKG_VERSION:=1
PKG_RELEASE:=1

FW_SOURCE:=danube_firmware.bin
FW_URL:=http://www.arcor.de/hilfe/files/pdf/
FW_FILE=arcor_A800_452CPW_FW_1.02.206(20081201).bin
FW_MD5SUM:=19d9af4e369287a0f0abaed415cdac10

include $(INCLUDE_DIR)/package.mk

define Download/firmware
  FILE:=$(FW_FILE)
  URL:=$(FW_URL)
  MD5SUM:=$(FW_MD5SUM)
endef
$(eval $(call Download,firmware))

define KernelPackage/lqtapi
  SUBMENU:=Voice over IP
  TITLE:=Tapi drivers for Lantiq SoC
  DEPENDS:=@TARGET_lantiq_xway @BROKEN
  FILES:=$(PKG_BUILD_DIR)/tapi/tapi.ko \
	$(PKG_BUILD_DIR)/mps/mps.ko \
  	$(PKG_BUILD_DIR)/mps/vmmc.ko
  AUTOLOAD:=$(call AutoLoad,80,tapi) \
	$(call AutoLoad,81,mps)
endef

define Package/lqtapi-firmware-danube
  SECTION:=sys
  CATEGORY:=Kernel modules
  SUBMENU:=Voice over IP
  TITLE:=Danube firmware
  DEPENDS:=+kmod-lqtapi
endef

include $(INCLUDE_DIR)/kernel-defaults.mk

define KernelPackage/lqtapi/description
	FOSS drivers for Lantiq SoC voip core
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) $(KERNEL_MAKEOPTS) \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS=-I$(PKG_BUILD_DIR)/include/ \
		modules
endef

define Package/lqtapi-firmware-danube/install
	$(TOPDIR)/target/linux/$(BOARD)/extract.sh $(DL_DIR) '$(FW_FILE)' 
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(INSTALL_DATA) $(DL_DIR)/voip.bin $(1)/lib/firmware/$(FW_SOURCE)
endef

$(eval $(call KernelPackage,lqtapi))
$(eval $(call BuildPackage,lqtapi-firmware-danube))
