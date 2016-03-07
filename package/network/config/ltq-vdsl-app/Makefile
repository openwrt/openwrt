# Copyright (C) 2010 OpenWrt.org
# Copyright (C) 2015 Lantiq Beteiligungs GmbH & Co KG.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ltq-vdsl-app
PKG_VERSION:=4.16.6.3
PKG_RELEASE:=2
PKG_BASE_NAME:=dsl_cpe_control
PKG_SOURCE:=$(PKG_BASE_NAME)_vrx-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources
PKG_MD5SUM:=8bedf330a456fe0864844e61b57da627
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_LICENSE:=BSD-2-Clause

PKG_BUILD_DEPENDS:=kmod-ltq-vdsl-vr9

include $(INCLUDE_DIR)/package.mk

define Package/ltq-vdsl-app
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Lantiq VDSL userland tool
  URL:=http://www.lantiq.com/
  DEPENDS:=@TARGET_lantiq_xrx200 +libpthread +librt +atm-esi
endef

define Package/ltq-vdsl-app/description
  Userland tool needed to control Lantiq VDSL CPE
endef

CONFIGURE_ARGS += \
	--enable-vrx \
	--enable-vrx-device=vr9 \
	--enable-driver-include="-I$(STAGING_DIR)/usr/include/drv_vdsl_cpe_api" \
	--enable-device-driver-include="-I$(STAGING_DIR)/usr/include/vdsl/" \
	--enable-ifxos \
	--enable-ifxos-include="-I$(STAGING_DIR)/usr/include/ifxos" \
	--enable-ifxos-library="-I$(STAGING_DIR)/usr/lib" \
	--enable-add-appl-cflags="-DMAX_CLI_PIPES=1"  \
	--enable-debug \
	--disable-dti \
	--with-channels-per-line="1" \

#CONFIGURE_ARGS += --enable-model=full
#CONFIGURE_ARGS += --enable-model=lite
#CONFIGURE_ARGS += --enable-model=footprint
CONFIGURE_ARGS += \
	--enable-model=typical \
	--enable-dsl-pm-showtime \
	--disable-dsl-ceoc
#CONFIGURE_ARGS += --enable-model=debug

define Package/ltq-vdsl-app/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/sbin
	$(INSTALL_BIN) ./files/dsl_control $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/vdsl_cpe_control_wrapper $(1)/sbin/

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/dsl_cpe_control $(1)/sbin/vdsl_cpe_control
	$(INSTALL_BIN) ./files/dsl_cpe_pipe.sh $(1)/sbin/
endef

$(eval $(call BuildPackage,ltq-vdsl-app))
