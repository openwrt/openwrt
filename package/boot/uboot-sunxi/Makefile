#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=u-boot
PKG_VERSION:=2013.10-rc2-sunxi
PKG_RELEASE:=1
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://github.com/linux-sunxi/u-boot-sunxi
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=b322f6723d59b9bb88701991c7b8b15867bf3692
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define uboot/Default
  TITLE:=
  CONFIG:=
  IMAGE:=
endef

define uboot/A10-OLinuXino-Lime
  TITLE:=U-Boot 2013.10-rc2 for the A10 OLinuXino LIME
endef

define uboot/A13-OLinuXino
  TITLE:=U-Boot 2013.10-rc2 for the A13 OlinuXino
endef

define uboot/A20-OLinuXino_MICRO
  TITLE:=U-Boot 2013.10-rc2 for A20 OLinuXino MICRO
endef

define uboot/Cubieboard
  TITLE:=U-Boot 2013.10-rc2 for Cubieboard
endef

define uboot/Cubieboard2
  TITLE:=U-Boot 2013.10-rc2 for Cubieboard2
endef

define uboot/Cubietruck
  TITLE:=U-Boot 2013.10-rc2 for Cubietruck
endef

define uboot/Hackberry
  TITLE:=U-Boot 2013.10-rc2 for the Hackbeery
endef

define uboot/pcDuino
  TITLE:=U-Boot 2013.10-rc2 for pcDuino
endef  

UBOOTS:=A10-OLinuXino-Lime A13-OLinuXino A20-OLinuXino_MICRO Cubieboard Cubieboard2 Cubietruck Hackberry pcDuino

define Package/uboot/template
define Package/uboot-sunxi-$(1)
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_sunxi
  TITLE:=$(2)
  URL:=http://www.denx.de/wiki/U-Boot
  VARIANT:=$(1)
  MAINTAINER:=Zoltan HERPAI <wigyori@uid0.hu>
endef
endef

define BuildUBootPackage
	$(eval $(uboot/Default))
	$(eval $(uboot/$(1)))
	$(call Package/uboot/template,$(1),$(TITLE))
endef

ifdef BUILD_VARIANT
$(eval $(call uboot/$(BUILD_VARIANT)))
UBOOT_CONFIG:=$(if $(CONFIG),$(CONFIG),$(BUILD_VARIANT))
UBOOT_IMAGE:=$(if $(IMAGE),$(IMAGE),openwrt-$(BOARD)-$(BUILD_VARIANT)-u-boot.bin)
endif

define Build/Configure
	$(MAKE) -C $(PKG_BUILD_DIR) \
		USE_PRIVATE_LIBGCC=yes $(UBOOT_CONFIG)_config
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CROSS_COMPILE=$(TARGET_CROSS)
endef

define Package/uboot/install/default
	$(INSTALL_DIR) $(BIN_DIR)/uboot-$(BOARD)-$(1)
	$(CP) $(PKG_BUILD_DIR)/u-boot.bin \
		$(BIN_DIR)/uboot-$(BOARD)-$(1)/openwrt-$(BOARD)-$(1)-u-boot.bin
	$(CP) $(PKG_BUILD_DIR)/spl/sunxi-spl.bin \
		$(BIN_DIR)/uboot-$(BOARD)-$(1)/openwrt-$(BOARD)-$(1)-spl.bin
	$(CP) $(PKG_BUILD_DIR)/u-boot-sunxi-with-spl.bin \
		$(BIN_DIR)/uboot-$(BOARD)-$(1)/openwrt-$(BOARD)-$(1)-u-boot-with-spl.bin
	$(CP) uEnv.txt \
		$(BIN_DIR)/uboot-$(BOARD)-$(1)/openwrt-$(BOARD)-$(1)-uEnv.txt
endef

define Package/uboot/install/template
define Package/uboot-sunxi-$(1)/install
	$(call Package/uboot/install/default,$(2))
endef
endef

$(foreach u,$(UBOOTS), \
	$(eval $(call Package/uboot/install/template,$(u),$(u))) \
)

$(foreach u,$(UBOOTS), \
	$(eval $(call BuildUBootPackage,$(u))) \
	$(eval $(call BuildPackage,uboot-sunxi-$(u))) \
)
