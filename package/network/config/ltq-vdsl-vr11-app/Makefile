# Copyright (C) 2010 OpenWrt.org
# Copyright (C) 2015-2016 Lantiq Beteiligungs GmbH & Co KG.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ltq-vdsl-vr11-app
PKG_VERSION:=4.23.1
PKG_RELEASE:=2
PKG_BASE_NAME:=dsl_cpe_control

UGW_VERSION=8.5.2.10
UGW_BASENAME=$(PKG_BASE_NAME)-ugw_$(UGW_VERSION)

PKG_SOURCE:=$(UGW_BASENAME).tar.bz2
PKG_SOURCE_URL:=https://gitlab.com/prpl-foundation/intel/$(PKG_BASE_NAME)/-/archive/ugw_$(UGW_VERSION)/
PKG_HASH:=d21ec74ca30f7f3893a8aa26d2b74ec319652f6b112832efab6f1274c7e5d1fc
PKG_BUILD_DIR:=$(BUILD_DIR)/$(UGW_BASENAME)
PKG_LICENSE:=BSD-2-Clause
PKG_LICENSE_FILES:=LICENSE

PKG_BUILD_DEPENDS:=ltq-vdsl-vr11

PKG_FLAGS:=nonshared
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/ltq-vdsl-vr11-app
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Lantiq VDSL userland tool
  URL:=http://www.lantiq.com/
  DEPENDS:=@TARGET_ipq40xx +libpthread +librt +libubox +libubus +ltq-dsl-base +kmod-ltq-vdsl-vr11
  PROVIDES:=ltq-dsl-app
endef

define Package/ltq-vdsl-vr11-app/description
  Userland tool needed to control Lantiq VDSL CPE
endef

# ltq-vdsl-vr11-app uses a header provided by the MEI driver which has some
# conditionals.
#
# Define them here with the default values they would get in the MEI driver,
# have the same view on both sides.
#
# If you change them, you need to change them for the ltq-vdsl-vr11-app as well
VDSL_APP_CFLAGS = \
	-DMAX_CLI_PIPES=1 \
	-DMEI_SUPPORT_DEBUG_STREAMS=1 \
	-DMEI_SUPPORT_OPTIMIZED_FW_DL=1

CONFIGURE_ARGS += \
	--enable-debug-logger-support=no

CONFIGURE_ARGS += \
	--enable-vrx \
	--enable-vrx-device=vr11 \
	--enable-driver-include="-I$(STAGING_DIR)/usr/include/drv_vdsl_cpe_api" \
	--enable-device-driver-include="-I$(STAGING_DIR)/usr/include/vdsl/" \
	--enable-ifxos \
	--enable-ifxos-include="-I$(STAGING_DIR)/usr/include/ifxos" \
	--enable-ifxos-library="-I$(STAGING_DIR)/usr/lib" \
	--enable-add-appl-cflags="$(VDSL_APP_CFLAGS)"  \
	--enable-debug \
	--disable-dti

CONFIGURE_ARGS += \
	--enable-model=full \
	--enable-dsl-ceoc=no
#CONFIGURE_ARGS += --enable-model=lite
#CONFIGURE_ARGS += --enable-model=footprint
#CONFIGURE_ARGS += --enable-model=typical
#CONFIGURE_ARGS += --enable-model=debug

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) ../ltq-vdsl-vr9-app/src/src/dsl_cpe_ubus.c $(PKG_BUILD_DIR)/src/
endef

define Package/ltq-vdsl-vr11-app/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/sbin $(1)/etc/hotplug.d/dsl
	$(INSTALL_BIN) ./files/dsl_control $(1)/etc/init.d/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/dsl_cpe_control $(1)/sbin/vdsl_cpe_control
	$(INSTALL_BIN) ./files/dsl_cpe_pipe.sh $(1)/sbin/
endef

$(eval $(call BuildPackage,ltq-vdsl-vr11-app))
