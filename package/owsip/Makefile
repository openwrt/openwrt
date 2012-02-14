#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

OWSIP_VERSION=2012-02-14
OWSIP_RELEASE=1

PKG_NAME:=owsip
PKG_VERSION:=$(OWSIP_VERSION)$(if $(OWSIP_RELEASE),.$(OWSIP_RELEASE))
PKG_RELEASE:=1
PKG_REV:=da53a53db28b47ca1714ffba72d0df5bea357706

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://nbd.name/owsip.git
PKG_SOURCE_SUBDIR:=owsip-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=git
PKG_MIRROR_MD5SUM:=74b0ab930321c4f85f220ff3852e210a

include $(INCLUDE_DIR)/package.mk

define Package/owsip-template
  SUBMENU:=Telephony
  SECTION:=net
  CATEGORY:=Network
  TITLE:=owsip using $(2)
  VARIANT:=$(1)
  DEPENDS:=+librt +libuci +libubox +pjsip-$(1) $(3)
endef

Package/owsip-oss=$(call Package/owsip-template,oss,OSS,BROKEN)
Package/owsip-ltq-tapi=$(call Package/owsip-template,ltq-tapi,Lantiq VMMC,@TARGET_lantiq +kmod-ltq-kpi2udp)

define Package/owsip-$(BUILD_VARIANT)/description
	OpenWrt sip daemon - $(BUILD_VARIANT)
endef

USE_LOCAL=$(shell ls ./src/ 2>/dev/null >/dev/null && echo 1)
ifneq ($(USE_LOCAL),)
define Build/Prepare
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef
endif

EXTRA_CFLAGS=-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include \
	-I$(STAGING_DIR)/usr/pjsip-$(BUILD_VARIANT)/include
EXTRA_LDFLAGS=-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/usr/pjsip-$(BUILD_VARIANT)/lib

define Build/Compile
	PKG_CONFIG_PATH=$(STAGING_DIR)/usr/pjsip-$(BUILD_VARIANT)/lib/pkgconfig \
		BACKEND=$(BUILD_VARIANT) CFLAGS="$(EXTRA_CFLAGS)" LDFLAGS="$(EXTRA_LDFLAGS)" $(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
endef

define Package/owsip-$(BUILD_VARIANT)/conffiles
/etc/config/telephony.conf
endef

define Package/owsip-$(BUILD_VARIANT)/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/init.d $(1)/etc/config $(1)/etc/uci-defaults
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/owsip_ua $(1)/usr/bin
	$(INSTALL_BIN) ./files/telephony.init $(1)/etc/init.d/telephony
	$(INSTALL_DATA) ./files/telephony.conf $(1)/etc/config/telephony
	$(INSTALL_DATA) ./files/telephony.defaults $(1)/etc/uci-defaults/telephony
endef

$(eval $(call BuildPackage,owsip-oss))
$(eval $(call BuildPackage,owsip-ltq-tapi))
