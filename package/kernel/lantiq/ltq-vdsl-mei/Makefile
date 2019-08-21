# Copyright (C) 2012 OpenWrt.org
# Copyright (C) 2015-2016 Lantiq Beteiligungs GmbH & Co KG.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_BASE_NAME:=drv_mei_cpe

ifeq ($(CONFIG_TARGET_lantiq_xrx200),y)
DEVICE=vr9
PKG_VERSION:=1.5.17.6
PKG_RELEASE:=4
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources
PKG_HASH:=94f6904364348b7f74087e721968abc28b2564fb9bd8899aa930d36490387662
PATCH_DIR:=patches
endif

ifeq ($(CONFIG_TARGET_ipq40xx),y)
DEVICE=vr11
PKG_VERSION:=1.9.3
PKG_RELEASE:=1
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.prpl.dev/intel/dsl_cpe_mei.git
PKG_SOURCE_DATE:=2019-06-09
PKG_SOURCE_VERSION:=40697fff6422d8e888590927481063b7ff288d53
PKG_SOURCE_SUBDIR:=$(PKG_BASE_NAME)-1.9.1
PKG_SOURCE:=$(PKG_BASE_NAME)-1.9.1.tar.gz
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_UNPACK=$(HOST_TAR) -C $(PKG_BUILD_DIR) -xzf $(DL_DIR)/$(PKG_SOURCE) --strip 1
PKG_HASH:=fa49740c6678c14c24da88298162595ee9f5b1f16f974456fd616fee105fe410
PATCH_DIR:=patches-$(PKG_VERSION)
endif

PKG_NAME:=ltq-vdsl-$(DEVICE)-mei

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_FIXUP:=autoreconf
PKG_FLAGS:=nonshared
PKG_MAINTAINER:=John Crispin <john@phrozen.org>
PKG_LICENSE:=GPL-2.0 BSD-2-Clause
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ltq-vdsl-vr9-mei
  TITLE:=mei driver for vdsl
  SECTION:=sys
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_lantiq_xrx200 +kmod-ltq-ifxos
  FILES:=$(PKG_BUILD_DIR)/src/drv_mei_cpe.ko
  AUTOLOAD:=$(call AutoLoad,50,drv_mei_cpe)
endef

define KernelPackage/ltq-vdsl-vr9-mei/description
	Lantiq MEI CPE Kernel Module Driver
endef


define KernelPackage/ltq-vdsl-vr11-mei
  TITLE:=mei driver for vdsl
  SECTION:=sys
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_ipq40xx +kmod-ltq-ifxos +kmod-vrx518_tc
  FILES:=$(PKG_BUILD_DIR)/src/drv_mei_cpe.ko
  AUTOLOAD:=$(call AutoLoad,50,drv_mei_cpe)
endef

define KernelPackage/ltq-vdsl-vr11-mei/description
	Lantiq MEI CPE Kernel Module Driver
endef


define Package/ltq-vdsl-mei_test
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Lantiq mei driver test tool
  URL:=http://www.lantiq.com/
  DEPENDS:=@TARGET_ipq40xx||TARGET_lantiq_xrx200
endef

define Package/ltq-vdsl-mei_test/description
	Userland tool to directly control the mei driver, this is only needed
	for test and development purposes.
endef

MAKE_FLAGS += \
	SHELL="$(BASH)"

ifeq ($(PKG_VERSION),1.5.17.6)
CONFIGURE_ARGS += \
	--with-max-device=1 \
	--with-lines-per-device=1

MEI_DRV_CFLAGS+= \
	-DMEI_DRV_ATM_PTM_INTERFACE_ENABLE=1
endif

ifeq ($(PKG_VERSION),1.9.3)
CONFIGURE_ARGS += \
	--enable-debug-logger-support=no \
	--enable-debug-stream-support=no

# MEI_DRV_CFLAGS+= \
# 	-DMEI_SUPPORT_OPTIMIZED_FW_DL=0 \
# 	-DIRQ_POLLING_FORCE=99
endif

# This looks weird, but it's necessary to address the right device.
# (pdev->dev.parent instead of pdev->dev)
ifeq ($(CONFIG_TARGET_ipq40xx),y)
MEI_DRV_CFLAGS+= \
	-DMEI_TARGET_x86=1
endif

ifeq ($(DEVICE),vr9)
CONFIGURE_ARGS += \
	--disable-ppa-callbacks-tc
endif

CONFIGURE_ARGS += \
	--enable-kernelincl="$(LINUX_DIR)/include" \
	--enable-device=$(DEVICE) \
	--enable-debug \
	--enable-error_print \
	--enable-ifxos-include="-I$(STAGING_DIR)/usr/include/ifxos/" \
	--enable-ifxos-library="-L$(STAGING_DIR)/usr/lib" \
	--enable-add_drv_cflags="${MEI_DRV_CFLAGS}" \
	--enable-linux-26 \
	--enable-kernelbuild="$(LINUX_DIR)" \
	--enable-drv_test_appl=yes \
	ARCH=$(LINUX_KARCH)

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/vdsl
	$(CP) $(PKG_BUILD_DIR)/src/drv_mei_cpe_api_intern.h $(1)/usr/include/vdsl/
	$(CP) $(PKG_BUILD_DIR)/src/drv_mei_cpe_api_atm_ptm_intern.h $(1)/usr/include/vdsl/
	$(CP) $(PKG_BUILD_DIR)/src/drv_mei_cpe_interface.h $(1)/usr/include/vdsl
	$(CP) $(PKG_BUILD_DIR)/src/drv_mei_cpe_config.h $(1)/usr/include/vdsl/
	$(CP) $(PKG_BUILD_DIR)/src/cmv_message_format.h $(1)/usr/include/vdsl/
endef

$(eval $(call KernelPackage,ltq-vdsl-vr9-mei))
$(eval $(call KernelPackage,ltq-vdsl-vr11-mei))

define Package/ltq-vdsl-mei_test/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/mei_cpe_drv_test $(1)/bin
endef

$(eval $(call BuildPackage,ltq-vdsl-mei_test))
