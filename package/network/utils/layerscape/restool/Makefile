#
# Copyright 2017 NXP
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=restool
PKG_VERSION:=2017.09
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/qoriq-open-source/restool.git
PKG_SOURCE_VERSION:=8f08b9d499e84f9057784c2036f0ddf75ae3fc70
PKG_MIRROR_HASH:=ff672b420041deac190cc9ddf314c52ec325567c429bc0aca3c692b2c7dc818f

include $(INCLUDE_DIR)/package.mk

define Package/restool
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=@TARGET_layerscape
  TITLE:=Layerscape DPAA2 dynamical management tool
endef

MAKE_FLAGS += \
	DESTDIR="$(PKG_BUILD_DIR)"/output/ \
	install

define Package/restool/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/output/usr/local/bin/* $(1)/usr/bin/
endef

$(eval $(call BuildPackage,restool))
