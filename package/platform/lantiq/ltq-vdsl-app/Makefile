# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ltq-vdsl-app
PKG_VERSION:=4.11.4
PKG_RELEASE:=1
PKG_BASE_NAME:=dsl_cpe_control_vrx
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/dsl_cpe_control-$(PKG_VERSION)

PKG_BUILD_DEPENDS:=kmod-ltq-vdsl-vr9

include $(INCLUDE_DIR)/package.mk

define Package/ltq-vdsl-app
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Lantiq VDSL userland tool
  URL:=http://www.lantiq.com/
  DEPENDS:=@TARGET_lantiq_xway +libpthread
endef

define Package/ltq-vdsl-app/description
  Userland tool needed to control Lantiq VDSL CPE
endef

CONFIGURE_ARGS += \
	--with-max-device="1" \
	--with-lines-per-device="1" \
	--with-channels-per-line="1" \
	--enable-vrx \
	--enable-driver-include="-I$(STAGING_DIR)/usr/include/drv_vdsl_cpe_api" \
	--enable-device-driver-include="-I$(STAGING_DIR)/usr/include/vdsl/" \
	--enable-add-appl-cflags="-DMAX_CLI_PIPES=2" \
	--enable-ifxos \
	--enable-ifxos-include="-I$(STAGING_DIR)/usr/include/ifxos" \
	--enable-ifxos-library="-I$(STAGING_DIR)/usr/lib" \
	--enable-dsl-ceoc \
	--enable-dsl-pm-total \
	--enable-dsl-pm-showtime \
	--enable-dsl-pm-line-counters \
	--enable-dsl-pm-line-failure-counters \
	--enable-dsl-pm-datapath-counters \
	--enable-dsl-pm-datapath-failure-counters \
	--enable-deprecated \
	--disable-soap-support \
	--enable-dsl-bonding=no \
	--disable-dti

ifeq ($(CONFIG_IFX_CLI),y)
CONFIGURE_ARGS += \
	--enable-cli-support
endif

CONFIGURE_ARGS += --enable-model=full
#CONFIGURE_ARGS += --enable-model=lite
#CONFIGURE_ARGS += --enable-model=footprint
#CONFIGURE_ARGS += --enable-model=typical
#CONFIGURE_ARGS += --enable-model=debug

define Package/ltq-vdsl-app/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/dsl_control $(1)/etc/init.d/

	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/dsl_cpe_control $(1)/sbin/vdsl_cpe_control
	$(INSTALL_BIN) ./files/dsl_notify.sh $(1)/sbin
endef

$(eval $(call BuildPackage,ltq-vdsl-app))
