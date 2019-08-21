# Copyright (C) 2010 OpenWrt.org
# Copyright (C) 2015-2016 Lantiq Beteiligungs GmbH & Co KG.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_BASE_NAME:=dsl_cpe_control

ifeq ($(CONFIG_TARGET_lantiq_xrx200),y)
DEVICE=vr9
PKG_VERSION:=4.17.18.6
PKG_RELEASE:=3
PKG_SOURCE:=$(PKG_BASE_NAME)_vrx-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources
PKG_HASH:=da8bb929526a61aea0e153ef524331fcd472a1ebbc6d88ca017735a4f82ece02
PATCH_DIR:=patches
endif

ifeq ($(CONFIG_TARGET_ipq40xx),y)
DEVICE=vr11
PKG_VERSION:=4.21.3
PKG_RELEASE:=1
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.prpl.dev/intel/dsl_cpe_control.git
PKG_SOURCE_DATE:=2019-06-09
PKG_SOURCE_VERSION:=f075b2449564809159cfed51a1f92e2f50ef0793
PKG_SOURCE_SUBDIR:=$(PKG_BASE_NAME)-4.21.1
PKG_SOURCE:=$(PKG_BASE_NAME)_vrx-4.21.1.tar.gz
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_UNPACK=$(HOST_TAR) -C $(PKG_BUILD_DIR) -xzf $(DL_DIR)/$(PKG_SOURCE) --strip 1
PKG_HASH:=a8b890c7187bd9bc7dcf0ca601fc88aa1a8246ee628e5d573f9dd9a17679c60b
PATCH_DIR:=patches-$(PKG_VERSION)
endif

PKG_NAME:=ltq-vdsl-app
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_LICENSE:=BSD-2-Clause

PKG_BUILD_DEPENDS:=ltq-vdsl

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/ltq-vdsl-app
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Lantiq VDSL userland tool
  URL:=http://www.lantiq.com/
  DEPENDS:=@TARGET_ipq40xx||TARGET_lantiq_xrx200 +libpthread +librt +ltq-xdsl-base-files
endef

define Package/ltq-vdsl-app/description
  Userland tool needed to control Lantiq VDSL CPE
endef

ifeq ($(PKG_VERSION),4.17.18.6)
CONFIGURE_ARGS += \
	--enable-add-appl-cflags="-DMAX_CLI_PIPES=1"  \
	--with-channels-per-line="1"
endif

ifeq ($(PKG_VERSION),4.21.3)
CONFIGURE_ARGS += \
	--enable-add-appl-cflags="-DMAX_CLI_PIPES=1 -DMEI_SUPPORT_DEBUG_STREAMS=0 -DMEI_SUPPORT_OPTIMIZED_FW_DL=1"  \
	--enable-debug-logger-support=no
endif

CONFIGURE_ARGS += \
	--enable-vrx \
	--enable-vrx-device=$(DEVICE) \
	--enable-driver-include="-I$(STAGING_DIR)/usr/include/drv_vdsl_cpe_api" \
	--enable-device-driver-include="-I$(STAGING_DIR)/usr/include/vdsl/" \
	--enable-ifxos \
	--enable-ifxos-include="-I$(STAGING_DIR)/usr/include/ifxos" \
	--enable-ifxos-library="-I$(STAGING_DIR)/usr/lib" \
	--enable-debug \
	--disable-dti \

#CONFIGURE_ARGS += --enable-model=full
#CONFIGURE_ARGS += --enable-model=lite
#CONFIGURE_ARGS += --enable-model=footprint
CONFIGURE_ARGS += \
	--enable-model=typical \
	--enable-dsl-pm-showtime \
	--disable-dsl-ceoc
#CONFIGURE_ARGS += --enable-model=debug

define Package/ltq-vdsl-app/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/sbin $(1)/etc/hotplug.d/dsl
	$(INSTALL_BIN) ./files/dsl_control $(1)/etc/init.d/
ifeq ($(DEVICE),vr9)
	$(INSTALL_BIN) ./files/10_atm.sh $(1)/etc/hotplug.d/dsl
	$(INSTALL_BIN) ./files/10_ptm.sh $(1)/etc/hotplug.d/dsl
endif
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/dsl_cpe_control $(1)/sbin/vdsl_cpe_control
	$(INSTALL_BIN) ./files/dsl_cpe_pipe.sh $(1)/sbin/
endef

$(eval $(call BuildPackage,ltq-vdsl-app))
