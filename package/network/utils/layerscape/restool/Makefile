#
# Copyright 2017 NXP
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=restool
PKG_VERSION:=LSDK-20.04
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://source.codeaurora.org/external/qoriq/qoriq-components/restool
PKG_SOURCE_VERSION:=f0cec094e4c6d1c975b377203a3bf994ba9325a9
PKG_MIRROR_HASH:=1863acfaef319e6b277671fead51df0a31bdddb59022080d86b7d81da0bc8490

include $(INCLUDE_DIR)/package.mk

define Package/restool
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Layerscape DPAA2 dynamical management tool
  DEPENDS:=@TARGET_layerscape_armv8_64b
endef

define Package/restool/config
  source "$(SOURCE)/Config.in"
endef

define Build/Compile
	cd $(PKG_BUILD_DIR); \
	sed -i 's/v1.6/$(PKG_VERSION)/g' Makefile; \
	sed -i 's/get_label() {/get_label() { usleep 80000/g' scripts/ls-main; \
	$(MAKE) CROSS_COMPILE=$(TARGET_CROSS) DESTDIR=$(PKG_BUILD_DIR)/output/ install
endef

define Package/restool/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/output/usr/local/bin/* $(1)/usr/bin/
endef

$(eval $(call BuildPackage,restool))
