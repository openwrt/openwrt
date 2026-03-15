#
# Copyright 2017 NXP
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=restool
PKG_VERSION:=6.6.23.2.0.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/nxp-qoriq/restool
PKG_SOURCE_VERSION:=lf-6.6.23-2.0.0
PKG_MIRROR_HASH:=e669c9faf5a00e36d763dbbd6dd866d878b0101df086582c38908d281fa3b465

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/restool
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Layerscape DPAA2 dynamical management tool
  DEPENDS:=@TARGET_layerscape_armv8_64b
endef

MAKE_FLAGS += \
	DESTDIR="$(PKG_BUILD_DIR)"/output/ \
	install

define Package/restool/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/output/usr/local/bin/* $(1)/usr/bin/
endef

$(eval $(call BuildPackage,restool))
