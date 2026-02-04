#
# Copyright (C) 2019 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=vrx518_tc
PKG_VERSION:=1.5.12.4
PKG_RELEASE:=4
PKG_BASE_NAME:=vrx518_tc_drv

UGW_VERSION=8.5.2.10
UGW_BASENAME=$(PKG_BASE_NAME)-ugw_$(UGW_VERSION)

PKG_SOURCE:=$(UGW_BASENAME).tar.bz2
PKG_SOURCE_URL:=https://gitlab.com/prpl-foundation/intel/$(PKG_BASE_NAME)/-/archive/ugw_$(UGW_VERSION)/
PKG_HASH:=0c5bb0f9a06dc4cc4bbb8b930d01a673daba1b66615e8328818356d32c8f1548
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(UGW_BASENAME)
PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

PLAT_DIR:=dcdp
PKG_EXTMOD_SUBDIRS:=$(PLAT_DIR)

define KernelPackage/$(PKG_NAME)
  SECTION:=sys
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=VRX518 TC driver
  KCONFIG:= \
    CONFIG_ATM_LANE=m \
    CONFIG_ATM_MPOA=m \
    CONFIG_ATM_MPOA_INTEL_DSL_PHY_SUPPORT=y
  DEPENDS:=@TARGET_ipq40xx +kmod-vrx518_ep +vrx518_ppe_fw +kmod-crypto-md5 +kmod-atm +kmod-ipoa +br2684ctl
  AUTOLOAD:=$(call AutoLoad,27,vrx518_tc)
  FILES:=$(PKG_BUILD_DIR)/$(PLAT_DIR)/$(PKG_NAME).ko
endef

define KernelPackage/$(PKG_NAME)/description
  VRX518 TC Driver
endef

define Build/Prepare
	$(PKG_UNPACK)
	# eliminate all carriage returns / convert to unix encoding
	(cd $(PKG_BUILD_DIR) && find . -type f -exec sed -i 's/\r//g' {} +)
	$(Build/Patch)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/net/
	$(CP) $(PKG_BUILD_DIR)/$(PLAT_DIR)/inc/dsl_tc.h $(1)/usr/include/net/
endef

EXTRA_CFLAGS:= \
	-I$(STAGING_DIR)/usr/include

define Build/Compile
	$(KERNEL_MAKE) \
		M="$(PKG_BUILD_DIR)/$(PLAT_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		FEATURE_VRX518_CPU=y \
		modules
endef

$(eval $(call KernelPackage,$(PKG_NAME)))
