#
# Copyright (C) 2013-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=u-boot
PKG_VERSION:=2015.01-rc3
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
	http://mirror2.openwrt.org/sources \
	ftp://ftp.denx.de/pub/u-boot
PKG_SOURCE_VERSION:=e680848838ca20a60d43b66a218d865b
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

PKG_LICENSE:=GPL-2.0 GPL-2.0+
PKG_LICENSE_FILES:=Licenses/README

include $(INCLUDE_DIR)/package.mk

define uboot/Default
  TITLE:=
  CONFIG:=
  IMAGE:=
endef

define uboot/A10-OLinuXino-Lime
  TITLE:=U-Boot for the A10 OLinuXino LIME
endef

define uboot/A13-OLinuXino
  TITLE:=U-Boot for the A13 OlinuXino
endef

define uboot/A20-OLinuXino-Lime
  TITLE:=U-Boot for the A20 OLinuXino LIME
endef

define uboot/A20-OLinuXino_MICRO
  TITLE:=U-Boot for A20 OLinuXino MICRO
endef

define uboot/Bananapi
  TITLE:=U-Boot for Bananapi
endef

define uboot/Bananapro
  TITLE:=U-Boot for Bananapro
endef

define uboot/Cubieboard
  TITLE:=U-Boot for Cubieboard
endef

define uboot/Cubieboard2
  TITLE:=U-Boot for Cubieboard2
endef

define uboot/Cubietruck
  TITLE:=U-Boot for Cubietruck
endef

define uboot/Hackberry
  TITLE:=U-Boot for the Hackbeery
endef

define uboot/Mele_M9
  TITLE:=U-Boot for the Mele M9 (A31)
endef

define uboot/OLIMEX-A13-SOM
  TITLE:=U-Boot for the Olimex A13 SOM
endef

define uboot/Linksprite_pcDuino
  TITLE:=U-Boot for Linksprite pcDuino
endef  

define uboot/Linksprite_pcDuino3
  TITLE:=U-Boot for Linksprite pcDuino3
endef  

UBOOTS:= \
	A10-OLinuXino-Lime \
	A13-OLinuXino \
	A20-OLinuXino-Lime \
	A20-OLinuXino_MICRO \
	Bananapi \
	Bananapro \
	Cubieboard \
	Cubieboard2 \
	Cubietruck \
	Hackberry \
	Mele_M9 \
	OLIMEX-A13-SOM \
	Linksprite_pcDuino \
	Linksprite_pcDuino3 \

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
		USE_PRIVATE_LIBGCC=yes $(UBOOT_CONFIG)_defconfig
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
	mkimage -C none -A arm -T script -d $(BIN_DIR)/uboot-$(BOARD)-$(1)/openwrt-$(BOARD)-$(1)-uEnv.txt $(BIN_DIR)/uboot-$(BOARD)-$(1)/openwrt-$(BOARD)-$(1)-boot.scr
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
