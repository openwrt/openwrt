#
# Copyright 2017 NXP
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fmc-eth-config
PKG_SOURCE_DATE:=2016-12-22
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/qoriq-open-source/eth-config.git
PKG_SOURCE_VERSION:=e3dcd110638dab004bcc759f6f51a0994bdfd8d5
PKG_MIRROR_HASH:=6e1b65b4c0d1d62731054c130e5274b08bc091bac8d5544a54847b67eb675390

include $(INCLUDE_DIR)/package.mk

define Package/fmc-eth-config
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=@TARGET_layerscape
  TITLE:=Layerscape Ethernet Configuration Files
endef

define Build/Compile
endef

define Package/fmc-eth-config/install
	$(INSTALL_DIR) $(1)/etc/fmc/config/
	$(CP) $(PKG_BUILD_DIR)/obsolete $(1)/etc/fmc/config/
	$(CP) $(PKG_BUILD_DIR)/private $(1)/etc/fmc/config/
	$(CP) $(PKG_BUILD_DIR)/shared_mac $(1)/etc/fmc/config/
endef

$(eval $(call BuildPackage,fmc-eth-config))
