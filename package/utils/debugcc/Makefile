include $(TOPDIR)/rules.mk

PKG_NAME:=debugcc
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/linux-msm/debugcc.git
PKG_SOURCE_DATE:=2023-11-10
PKG_SOURCE_VERSION:=d15bea7c05f224a85dd1fcec24b0f153dbad9f6e
PKG_MIRROR_HASH:=b6bd181db4992cf429343cd7d0fdde0f937a8f1811f5fe9e7855a0d76b0d88a0

PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=LICENSE

PKG_MAINTAINER:=Christian Marangi <ansuelsmth@gmail.com>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/meson.mk

define Package/debugcc
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=QCOM debugcc utility
  DEPENDS:=@KERNEL_DEVMEM @(TARGET_qualcommax||TARGET_ipq806x||TARGET_ipq40xx)
endef

define Package/debugcc/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(MESON_BUILD_DIR)/debugcc $(1)/usr/bin/
endef

$(eval $(call BuildPackage,debugcc))
