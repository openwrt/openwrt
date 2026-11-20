# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2026.07
PKG_RELEASE:=1
PKG_HASH:=78e8bfc382fe388f9b55aa1daf8c563522a037779b5d4c349d1415e381f1243e

PKG_MAINTAINER:=Josef Schlehofer <pepe.schlehofer@gmail.com>

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=exynos
  UBOOT_IMAGE:=u-boot-dtb.bin
  HIDDEN:=1
endef

define U-Boot/odroid
  NAME:=Hardkernel ODROID-U2/U3/X2 (Exynos4412)
  BUILD_SUBTARGET:=cortexa9
  BUILD_DEVICES:= \
    hardkernel_odroid-u2 \
    hardkernel_odroid-u3 \
    hardkernel_odroid-x2
endef

UBOOT_TARGETS := odroid

UBOOT_CUSTOMIZE_CONFIG := \
	--disable TOOLS_MKEFICAPSULE

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(UBOOT_IMAGE)
endef

$(eval $(call BuildPackage/U-Boot))
