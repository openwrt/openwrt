#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_BASE_NAME:=dsl_cpe_control_danube
PKG_VERSION:=3.24.4.4
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/dsl_cpe_control-$(PKG_VERSION)
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources/
PKG_MD5SUM:=ee315306626b68794d3d3636dabfe161

include $(INCLUDE_DIR)/package.mk

define Package/ltq-dsl-app
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Lantiq DSL userland tool
  URL:=http://www.lantiq.com/
  DEPENDS:=@TARGET_lantiq_danube +kmod-ltq-dsl +libpthread
  MAINTAINER:=John Crispin <blogic@openwrt.org> 
endef

define Package/ltq-dsl-app/description
	Infineon DSL CPE API for Amazon SE, Danube and Vinax.
endef

IFX_DSL_MAX_DEVICE=1
IFX_DSL_LINES_PER_DEVICE=1
IFX_DSL_CHANNELS_PER_LINE=1
#CONFIG_IFX_CLI=y

CONFIGURE_ARGS += \
	--with-max-device="$(IFX_DSL_MAX_DEVICE)" \
	--with-lines-per-device="$(IFX_DSL_LINES_PER_DEVICE)" \
	--with-channels-per-line="$(IFX_DSL_CHANNELS_PER_LINE)" \
	--enable-danube \
	--enable-driver-include="-I$(STAGING_DIR)/usr/include" \
	--enable-debug-prints \
	--enable-add-appl-cflags="-DMAX_CLI_PIPES=2" \
	--enable-cmv-scripts \
	--enable-debug-tool-interface \
	--enable-adsl-led \
	--enable-dsl-ceoc \
	--enable-script-notification \
	--enable-dsl-pm \
	--enable-dsl-pm-total \
	--enable-dsl-pm-history \
	--enable-dsl-pm-showtime \
	--enable-dsl-pm-channel-counters \
	--enable-dsl-pm-datapath-counters \
	--enable-dsl-pm-line-counters \
	--enable-dsl-pm-channel-thresholds \
	--enable-dsl-pm-datapath-thresholds \
	--enable-dsl-pm-line-thresholds \
	--enable-dsl-pm-optional-parameters

ifeq ($(CONFIG_IFX_CLI),y)
CONFIGURE_ARGS += \
	--enable-cli-support \
	--enable-soap-support
endif

TARGET_CFLAGS += -I$(LINUX_DIR)/include

define Package/ltq-dsl-app/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ifx_cpe_control_init.sh $(1)/etc/init.d/

	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/dsl_cpe_control $(1)/sbin
endef

$(eval $(call BuildPackage,ltq-dsl-app))
