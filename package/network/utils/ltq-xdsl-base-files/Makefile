#
# Copyright (C) 2019 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ltq-xdsl-base-files
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/ltq-xdsl-base-files
  SECTION:=net
  CATEGORY:=Network
  TITLE:=xDSL related files for Intel/Lantiq DSL Chipsets
  URL:=http://openwrt.org/
  DEPENDS:=@TARGET_ipq40xx||TARGET_lantiq
endef

define Package/ltq-xdsl-base-files/description
  This package contains xDSL related files for Intel/Lantiq DSL Chipsets.
endef

define Build/Compile
endef

define Package/ltq-xdsl-base-files/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ltq-xdsl-base-files))
