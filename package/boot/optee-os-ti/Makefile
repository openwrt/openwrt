#
# Copyright (c) 2026 Aerlync Labs Inc.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_VERSION:=4.9.0
PKG_RELEASE:=1

PKG_HASH:=9400e16c45bfa45f15585b2c933b86c449e7de05def0ecaaa62a4f38973a3a45
PKG_MAINTAINER:=Sayooj K Karun <sayooj@aerlync.com>
PKG_LICENSE:=BSD-2-Clause
PKG_LICENSE_FILES:=LICENSE

OPTEE_USE_INTREE_DTC:=1

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/optee-os.mk
include $(INCLUDE_DIR)/package.mk

define Optee-os/Default
  BUILD_TARGET:=ti_k3
endef

define Optee-os/am62x
  BUILD_SUBTARGET:=am625
  NAME:=TI AM62x
  PLAT:=k3
  PLAT_FLAVOR:=am62x
  OPTEE_IMAGE:=tee-raw.bin
  OPTEE_MAKE_FLAGS += CFG_WITH_SOFTWARE_PRNG=y
endef

OPTEE_TARGETS := \
		 am62x

define Package/optee-os/install/default
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(foreach img,$(OPTEE_IMAGE), \
		$(CP) $(PKG_BUILD_DIR)/out/arm-plat-$(PLAT)/core/$(img) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(img); \
	)
endef

$(eval $(call BuildPackage/Optee-os))
