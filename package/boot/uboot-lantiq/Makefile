#
# Copyright (C) 2012-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=u-boot
PKG_VERSION:=2013.10
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
	http://mirror2.openwrt.org/sources \
	ftp://ftp.denx.de/pub/u-boot
PKG_MD5SUM:=a076a044b64371edc52f7e562b13f6b2
PKG_TARGETS:=bin

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define uboot/Default
  TITLE:=
  SOC:=
  DDR_SETTINGS:=
  IMAGE:=
endef

define uboot/arv4519pw_ram
  TITLE:=U-Boot for Arcadyan arv4519pw (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/arcadyan/arv4519pw/ddr_settings.h
endef

define uboot/arv4519pw_nor
  TITLE:=U-Boot for Arcadyan arv4519pw (NOR)
  SOC:=danube
endef

define uboot/arv4519pw_brn
  TITLE:=U-Boot for Arcadyan arv4519pw (BRN)
  SOC:=danube
endef

define uboot/arv7510pw_ram
  TITLE:=U-Boot for Arcadyan arv7510pw (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/arcadyan/arv7510pw/ddr_settings.h
endef

define uboot/arv7510pw_nor
  TITLE:=U-Boot for Arcadyan arv7510pw (NOR)
  SOC:=danube
endef

define uboot/arv7510pw_brn
  TITLE:=U-Boot for Arcadyan arv7510pw (BRN)
  SOC:=danube
endef

define uboot/arv7518pw_ram
  TITLE:=U-Boot for Arcadyan arv7518pw (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/arcadyan/arv7518pw/ddr_settings.h
endef

define uboot/arv7518pw_nor
  TITLE:=U-Boot for Arcadyan arv7518pw (NOR)
  SOC:=danube
endef

define uboot/arv7518pw_brn
  TITLE:=U-Boot for Arcadyan arv7518pw (BRN)
  SOC:=danube
endef

define uboot/arv752dpw_ram
  TITLE:=U-Boot for Arcadyan arv752dpw (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/arcadyan/arv752dpw/ddr_settings.h
endef

define uboot/arv752dpw_nor
  TITLE:=U-Boot for Arcadyan arv752dpw (NOR)
  SOC:=danube
endef

define uboot/arv752dpw_brn
  TITLE:=U-Boot for Arcadyan arv752dpw (BRN)
  SOC:=danube
endef

define uboot/arv752dpw22_ram
  TITLE:=U-Boot for Arcadyan arv752dpw22 (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/arcadyan/arv752dpw22/ddr_settings.h
endef

define uboot/arv752dpw22_nor
  TITLE:=U-Boot for Arcadyan arv752dpw22 (NOR)
  SOC:=danube
endef

define uboot/arv752dpw22_brn
  TITLE:=U-Boot for Arcadyan arv752dpw22 (BRN)
  SOC:=danube
endef

define uboot/gigasx76x_ram
  TITLE:=U-Boot for Siemens Gigaset sx76x (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/gigaset/sx76x/ddr_settings.h
endef

define uboot/gigasx76x_nor
  TITLE:=U-Boot for Siemens Gigaset sx76x (NOR)
  SOC:=danube
endef

define uboot/acmp252_ram
  TITLE:=U-Boot for AudioCodes MP-252 (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/audiocodes/acmp252/ddr_settings.h
endef

define uboot/acmp252_nor
  TITLE:=U-Boot for AudioCodes MP-252 (NOR)
  SOC:=danube
endef

define uboot/easy50712_ram
  TITLE:=U-Boot for Lantiq EASY50712 (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/lantiq/easy50712/ddr_settings.h
endef

define uboot/easy50712_nor
  TITLE:=U-Boot for Lantiq EASY50712 (NOR)
  SOC:=danube
endef

define uboot/easy50712_norspl
  TITLE:=U-Boot for Lantiq EASY50712 (NOR SPL)
  SOC:=danube
  IMAGE:=u-boot.ltq.lzo.norspl
endef

define uboot/easy80920_ram
  TITLE:=U-Boot for Lantiq EASY80920 (RAM)
  SOC:=vr9
  DDR_SETTINGS:=board/lantiq/easy80920/ddr_settings.h
endef

define uboot/easy80920_nor
  TITLE:=U-Boot for Lantiq EASY80920 (NOR)
  SOC:=vr9
endef

define uboot/easy80920_norspl
  TITLE:=U-Boot for Lantiq EASY80920 (NOR SPL)
  SOC:=vr9
  IMAGE:=u-boot.ltq.lzo.norspl
endef

define uboot/easy80920_sfspl
  TITLE:=U-Boot for Lantiq EASY80920 (SPI SPL)
  SOC:=vr9
  IMAGE:=u-boot.ltq.lzo.sfspl
endef


UBOOTS:= \
	arv4519pw_ram arv4519pw_nor arv4519pw_brn \
	arv7510pw_ram arv7510pw_nor arv7510pw_brn \
	arv7518pw_ram arv7518pw_nor arv7518pw_brn \
	arv752dpw_ram arv752dpw_nor arv752dpw_brn \
	arv752dpw22_ram arv752dpw22_nor arv752dpw22_brn \
	gigasx76x_ram gigasx76x_nor \
	acmp252_ram acmp252_nor \
	easy50712_ram easy50712_nor easy50712_norspl \
	easy80920_ram easy80920_nor easy80920_norspl easy80920_sfspl

define Package/uboot/template
define Package/uboot-lantiq-$(1)
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_lantiq_xway
  TITLE:=$(2)
  URL:=http://www.denx.de/wiki/U-Boot
  VARIANT:=$(1)
  MAINTAINER:=Luka Perkov <luka@openwrt.org>
endef
endef

define BuildUBootPackage
	$(eval $(uboot/Default))
	$(eval $(uboot/$(1)))
	$(call Package/uboot/template,$(1),$(TITLE))
endef

define Build/Configure
	$(MAKE) -C $(PKG_BUILD_DIR) $(BUILD_VARIANT)_config
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) CROSS_COMPILE=$(TARGET_CROSS)
endef

define Package/uboot/install/default
	$(CP) \
		$(PKG_BUILD_DIR)/$(2) \
		$(BIN_DIR)/uboot-$(BOARD)-$(1)/openwrt-$(BOARD)-$(1)-u-boot.img
endef

define Package/uboot/install/uart
	awk -f $(PKG_BUILD_DIR)/tools/lantiq_ram_init_uart.awk \
		-v soc=$(2) $(PKG_BUILD_DIR)/$(3) \
		> $(PKG_BUILD_DIR)/ddr_settings
	perl $(PKG_BUILD_DIR)/tools/gct.pl \
		$(PKG_BUILD_DIR)/ddr_settings $(PKG_BUILD_DIR)/u-boot.srec \
		$(BIN_DIR)/uboot-$(BOARD)-$(1)/openwrt-$(BOARD)-$(1)-u-boot.asc
 endef

define Package/uboot/install/template
define Package/uboot-lantiq-$(1)/install
	$(call Package/uboot/install/default,$(1),$(if $(IMAGE),$(IMAGE),u-boot.bin))
	$(if $(DDR_SETTINGS), \
		$(call Package/uboot/install/uart,$(1),$(SOC),$(DDR_SETTINGS)) \
	)
endef
endef

$(foreach u,$(UBOOTS), \
	$(eval $(call BuildUBootPackage,$(u))) \
	$(eval $(call Package/uboot/install/template,$(u))) \
	$(eval $(call BuildPackage,uboot-lantiq-$(u))) \
)
