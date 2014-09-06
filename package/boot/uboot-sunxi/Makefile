#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=u-boot
PKG_VERSION:=2014.04
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
	http://mirror2.openwrt.org/sources \
	ftp://ftp.denx.de/pub/u-boot
PKG_SOURCE_VERSION:=6d2116d1385a66e9a59742caa9d62a54
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define uboot/Default
  TITLE:=
  CONFIG:=
  IMAGE:=
endef

define uboot/A10-OLinuXino-Lime
  TITLE:=U-Boot 2014.04 for the A10 OLinuXino LIME
endef

define uboot/A13-OLinuXino
  TITLE:=U-Boot 2014.04 for the A13 OlinuXino
endef

define uboot/A20-OLinuXino_MICRO
  TITLE:=U-Boot 2014.04 for A20 OLinuXino MICRO
endef

define uboot/Bananapi
  TITLE:=U-Boot 2014.04 for Bananapi
endef

define uboot/Cubieboard
  TITLE:=U-Boot 2014.04 for Cubieboard
endef

define uboot/Cubieboard2
  TITLE:=U-Boot 2014.04 for Cubieboard2
endef

define uboot/Cubietruck
  TITLE:=U-Boot 2014.04 for Cubietruck
endef

define uboot/Hackberry
  TITLE:=U-Boot 2014.04 for the Hackbeery
endef

define uboot/pcDuino
  TITLE:=U-Boot 2014.04 for pcDuino
endef  

define uboot/Linksprite_pcDuino3
  TITLE:=U-Boot 2014.04 for Linksprite pcDuino3
endef  

UBOOTS:=A10-OLinuXino-Lime A13-OLinuXino A20-OLinuXino_MICRO Bananapi Cubieboard Cubieboard2 Cubietruck Hackberry pcDuino Linksprite_pcDuino3

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
