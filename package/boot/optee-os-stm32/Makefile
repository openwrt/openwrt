#
# Copyright (C) 2024 Bootlin
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_VERSION:=4.8.0
PKG_RELEASE:=1

PKG_HASH:=5222cd553f5edb69ae4ec7cb99b2bfec2c47a47c0be1865b49744701918e8b4d
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

define Optee-os/stm32mp157c-dk2
  $(call Optee-os/stm32mp1)
  NAME:=STM32MP157C-DK2
  PLAT_FLAVOR:=157C_DK2
endef

define Optee-os/stm32mp157c-dk2-scmi
  $(call Optee-os/stm32mp1)
  NAME:=STM32MP157C-DK2 (SCMI)
  PLAT_FLAVOR:=157C_DK2_SCMI
endef

OPTEE_TARGETS := \
		 stm32mp135f-dk \
		 stm32mp157c-dk2 \
		 stm32mp157c-dk2-scmi

define Package/optee-os/install/default
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(foreach img,$(OPTEE_IMAGE), \
		$(CP) $(PKG_BUILD_DIR)/out/arm-plat-$(PLAT)/core/$(img) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(img); \
	)
endef

$(eval $(call BuildPackage/Optee-os))
