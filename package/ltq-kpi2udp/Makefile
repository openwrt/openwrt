#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=drv_kpi2udp
PKG_VERSION:=2.2.0
PKG_RELEASE:=1

PKG_SOURCE:=drv_kpi2udp-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources
PKG_MD5SUM:=af3855609554c7f3d2c3df8c597f50a7

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ltq-kpi2udp
  SUBMENU:=Voice over IP
  TITLE:=TAPI KPI2UDP plug-in
  URL:=http://www.lantiq.com/
  DEPENDS:=+kmod-ltq-tapi @TARGET_lantiq
  FILES:=$(PKG_BUILD_DIR)/drv_kpi2udp.ko
  AUTOLOAD:=$(call AutoLoad,26,drv_kpi2udp)
  MAINTAINER:=John Crispin <blogic@openwrt.org>
endef

define KernelPackage/ltq-kpi2udp/description
	RTP packet path accelleration into IP stack (strongly recommended)
endef

CONFIGURE_ARGS += --enable-kernelincl="$(LINUX_DIR)/include" \
	--enable-tapiincl="$(STAGING_DIR)/usr/include/drv_tapi" \
	--with-ifxos-incl=$(STAGING_DIR)/usr/include/ifxos \
	--enable-warning \
	--enable-linux-26 \
	--enable-kernelbuild="$(LINUX_DIR)" \
	ARCH=$(LINUX_KARCH)

define Build/Configure
	(cd $(PKG_BUILD_DIR); aclocal && autoconf && automake)
	$(call Build/Configure/Default)
endef

$(eval $(call KernelPackage,ltq-kpi2udp))
