# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2022 OpenWrt.org
#

include $(TOPDIR)/rules.mk

PKG_NAME:=opensbi
PKG_VERSION:=1.9

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/riscv/opensbi
PKG_SOURCE_VERSION:=v$(PKG_VERSION)
PKG_MIRROR_HASH:=1abb1467ade206dc7880caf6bd994cac43cbb207882b43b63a05322d91ef3902

PKG_BUILD_DIR=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

PKG_TARGETS:=bin
PKG_FLAGS:=nonshared
PKG_LICENSE:=BSD-2-Clause
PKG_LICENSE_FILES:=COPYING.BSD
PKG_BUILD_PARALLEL:=1

PKG_MAINTAINER:=Zoltan HERPAI <wigyori@uid0.hu>

include $(INCLUDE_DIR)/package.mk

define Package/opensbi
    SECTION:=boot
    CATEGORY:=Boot Loaders
    DEPENDS:=@(TARGET_sifiveu||TARGET_d1||TARGET_spacemit)
    URL:=https://github.com/riscv/opensbi/blob/master/README.md
    VARIANT:=$(subst _,/,$(subst opensbi_,,$(1)))
    TITLE:=OpenSBI generic
    OPENSBI_IMAGE:=
    PLAT:=
endef

define Package/opensbi_generic
  $(Package/opensbi)
  TITLE:=OpenSBI generic
  OPENSBI_IMAGE:=fw_dynamic.bin
  PLAT:=generic
endef

export GCC_HONOUR_COPTS=s

MAKE_FLAGS = \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	REPRODUCIBLE=y \
	$(if $(findstring c,$(OPENWRT_VERBOSE)),V=1,V='') \
	OPENSBI_VERSION_GIT="v$(PKG_VERSION)-OpenWrt-$(REVISION)"

define Build/Compile
	$(eval $(Package/opensbi_$(BUILD_VARIANT))) \
		+$(MAKE) $(MAKE_FLAGS) -C $(PKG_BUILD_DIR) \
		PLATFORM=$(PLAT)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/build/platform/$(PLAT)/firmware/fw_dynamic.bin $(STAGING_DIR_IMAGE)/fw_dynamic-${BUILD_VARIANT}.bin
endef

$(eval $(call BuildPackage,opensbi_generic))
