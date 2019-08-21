#
# Copyright (C) 2019 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=vrx518_tc
PKG_VERSION:=1.5.12
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0 BSD-2-Clause
PKG_LICENSE_FILES:=LICENSE

PKG_BASE_NAME:=vrx518_tc_drv
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.prpl.dev/intel/vrx518_tc_drv.git
PKG_SOURCE_DATE:=2019-06-17
PKG_SOURCE_VERSION:=06287b08f8cd150238f537f81daf9e1c877b70c4
PKG_SOURCE_SUBDIR:=$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_HASH:=9afbc45890aaf9aefc9231eb868a9970289c3d337bb8a899eb19716085f6a0b9

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk


# ifneq ($(CONFIG_TARGET_intel_mips_xrx500),)
#     PLAT_DIR:=grx500
# else ifneq ($(CONFIG_TARGET_lantiq_xrx500),)
#     PLAT_DIR:=grx500
# else
    PLAT_DIR:=dcdp
# endif

define KernelPackage/$(PKG_NAME)
  SECTION:=sys
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=VRX518 TC driver
  KCONFIG:= \
    CONFIG_ATM_LANE=m \
    CONFIG_ATM_MPOA=m \
    CONFIG_ATM_MPOA_INTEL_DSL_PHY_SUPPORT=y
#   KCONFIG:= \
#     CONFIG_ATM=y \
#     CONFIG_ATM_BR2684=y \
#     CONFIG_ATM_LANE=y \
#     CONFIG_ATM_CLIP=y \
#     CONFIG_ATM_MPOA=y \
#     CONFIG_ATM_MPOA_INTEL_DSL_PHY_SUPPORT=y
# #     CONFIG_ATM_BR2684_MINI_JUMBO_FRAME_SUPPORT=y \
# #     CONFIG_LTQ_DATAPATH_ACA_CSUM_WORKAROUND=y
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

MAKE_OPTS:= \
	$(KERNEL_MAKE_FLAGS) \
	SUBDIRS="$(PKG_BUILD_DIR)/$(PLAT_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	"FEATURE_VRX518_CPU=y"

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,$(PKG_NAME)))
