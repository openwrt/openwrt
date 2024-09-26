#
# Copyright (C) 2024 Bootlin
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_VERSION:=4.3.0
PKG_RELEASE:=1

PKG_HASH:=390b271905c828d6def9fa6a77bbaa425f3b434d733c8eb18f582ccbc6896096
PKG_MAINTAINER:=Thomas Richard <thomas.richard@bootlin.com>

OPTEE_USE_INTREE_DTC:=1

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/optee-os.mk
include $(INCLUDE_DIR)/package.mk

define Optee-os/Default
  BUILD_TARGET:=stm32
endef

define Optee-os/stm32mp1
  BUILD_SUBTARGET:=stm32mp1
  PLAT:=stm32mp1
endef

define Optee-os/stm32mp135f-dk
  $(call Optee-os/stm32mp1)
  NAME:=STM32MP135F-DK
  PLAT_FLAVOR:=135F_DK
endef

OPTEE_TARGETS := stm32mp135f-dk

define Package/optee-os/install/default
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(foreach img,$(OPTEE_IMAGE), \
		$(CP) $(PKG_BUILD_DIR)/out/arm-plat-$(PLAT)/core/$(img) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(img); \
	)
endef

$(eval $(call BuildPackage/Optee-os))
