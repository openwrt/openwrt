#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=aarch64-boot-wrapper
PKG_VERSION:=2013-01-10
PKG_RELEASE:=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://git.kernel.org/pub/scm/linux/kernel/git/cmarinas/boot-wrapper-aarch64.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=c51dde817b5ed5b8f741b67ac51bd67bd87b4a2a
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_MAINTAINER:=Florian Fainelli <florian@openwrt.org>

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/image.mk

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		KERNEL="../Image" \
		DTC="$(LINUX_DIR)/scripts/dtc/dtc" \
		FDT_SRC="$(LINUX_DIR)/arch/$(LINUX_KARCH)/boot/dts/arm/foundation-v8.dts" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		BOOTARGS="console=ttyAMA0 earlyprintk"
endef

define Build/InstallDev
	$(CP) $(PKG_BUILD_DIR)/linux-system.axf $(KDIR)/linux-system.axf
endef

$(eval $(call Build/DefaultTargets))
