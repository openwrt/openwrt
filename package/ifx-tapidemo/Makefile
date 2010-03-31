#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=tapidemo
PKG_VERSION:=5.0.1.27
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources
PKG_MD5SUM:=a38a7bf3242aad607f50f57b988bc87c

include $(INCLUDE_DIR)/package.mk

define Package/ifx-tapidemo
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=TAPIdemo application for Lantiq boards
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq
  DEPENDS:=+kmod-ifx-tapi +kmod-ifx-vmmc @BROKEN
endef

define Package/ifx-tapidemo/description
	Voice Access mini-PBX Demo Application
endef

define Package/ifx-tapidemo/config
	if PACKAGE_ifx-tapidemo
		source "$(SOURCE)/Config.in"
	endif
endef

CONFIGURE_ARGS += \
	ARCH=$(LINUX_KARCH) \
	--enable-linux-26 \
	--enable-kernelincl="$(LINUX_DIR)/include" \
	--with-drvincl="$(STAGING_DIR)/usr/include" \
	$(call autoconf_bool,CONFIG_VOICE_CPE_TAPI_QOS,qos) \
	$(call autoconf_bool,CONFIG_VOICE_CPE_TAPIDEMO_FAX_T,fax-t38) \
	--enable-trace \
	--enable-fs

ifeq ($(CONFIG_VOICE_CPE_TAPIDEMO_BOARD_EASY50712),y)
  CONFIGURE_ARGS += --enable-boardname=EASY50712
endif
ifeq ($(CONFIG_VOICE_CPE_TAPIDEMO_BOARD_EASY50712_V3),y)
  CONFIGURE_ARGS += --enable-boardname=EASY50712_V3
endif
ifeq ($(CONFIG_VOICE_CPE_TAPIDEMO_BOARD_EASY508xx),y)
  CONFIGURE_ARGS += --enable-boardname=EASY508XX
endif
ifeq ($(CONFIG_VOICE_CPE_TAPIDEMO_BOARD_EASY80910),y)
  CONFIGURE_ARGS += --enable-boardname=EASY508XX
endif

define Package/ifx-tapidemo/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/tapidemo $(1)/usr/sbin
	$(INSTALL_BIN) ./files/bringup_tapidemo $(1)/etc/init.d/tapidemo
endef

$(eval $(call BuildPackage,ifx-tapidemo))
