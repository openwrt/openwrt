# Copyright (C) 2012 OpenWrt.org
# Copyright (C) 2015-2016 Lantiq Beteiligungs GmbH & Co KG.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ltq-vdsl-vr11-mei
PKG_VERSION:=1.11.1
PKG_RELEASE:=2
PKG_BASE_NAME:=dsl_cpe_mei

UGW_VERSION=8.5.2.10
UGW_BASENAME=$(PKG_BASE_NAME)-ugw_$(UGW_VERSION)

PKG_SOURCE:=$(UGW_BASENAME).tar.bz2
PKG_SOURCE_URL:=https://gitlab.com/prpl-foundation/intel/$(PKG_BASE_NAME)/-/archive/ugw_$(UGW_VERSION)/
PKG_HASH:=337614473d50ed64de010adaed99a16103e08eea8fc67fe9d6caf155bea33d1d
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(UGW_BASENAME)
PKG_LICENSE:=GPL-2.0 BSD-2-Clause
PKG_LICENSE_FILES:=LICENSE
PKG_EXTMOD_SUBDIRS:=src

PKG_FIXUP:=autoreconf
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

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


define Package/ltq-vdsl-vr11-mei-test
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Lantiq mei driver test tool
  URL:=http://www.lantiq.com/
  DEPENDS:=@TARGET_ipq40xx +kmod-ltq-vdsl-vr11-mei
endef

define Package/ltq-vdsl-vr11-mei-test/description
  Userland tool to directly control the mei driver, this is only needed
  for test and development purposes.
endef

MAKE_FLAGS += \
	$(KERNEL_MAKE_FLAGS) \
	SHELL="$(BASH)"

# ltq-vdsl-app uses a header provided by the MEI driver which has some
# conditionals.
# Define the conditionals here to have the same view on both sides. If you
# change them, you need to change them for the ltq-vdsl-app as well
MEI_DRV_CFLAGS = \
	-DMEI_DRV_ATM_PTM_INTERFACE_ENABLE=1 \
	-DMEI_SUPPORT_DEBUG_STREAMS=1 \
	-DMEI_SUPPORT_OPTIMIZED_FW_DL=1

#MEI_DRV_CFLAGS+= \
#	-DMEI_SUPPORT_OPTIMIZED_FW_DL=0 \
#	-DIRQ_POLLING_FORCE=99

CONFIGURE_ARGS += \
	--enable-debug-logger-support=no
#	--enable-debug-stream-support=no

# This looks weird, but it's necessary to address the right device.
# (pdev->dev.parent instead of pdev->dev)
MEI_DRV_CFLAGS+= \
	-DMEI_TARGET_x86=1

CONFIGURE_ARGS += \
	--enable-kernelincl="$(LINUX_DIR)/include" \
	--enable-device=vr11 \
	--enable-debug \
	--enable-error_print \
	--enable-ifxos-include="-I$(STAGING_DIR)/usr/include/ifxos/" \
	--enable-ifxos-library="-L$(STAGING_DIR)/usr/lib" \
	--enable-add_drv_cflags="$(MEI_DRV_CFLAGS)" \
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

$(eval $(call KernelPackage,ltq-vdsl-vr11-mei))

define Package/ltq-vdsl-vr11-mei-test/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/mei_cpe_drv_test $(1)/bin
endef

$(eval $(call BuildPackage,ltq-vdsl-vr11-mei-test))
