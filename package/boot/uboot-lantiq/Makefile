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

PKG_BUILD_PARALLEL:=1

FIRMWARE_LANTIQ_SOURCE:=$(TOPDIR)/target/linux/lantiq/files/firmware/lantiq

include $(INCLUDE_DIR)/package.mk

define uboot/Default
  TITLE:=
  SOC:=
  DDR_SETTINGS:=
  IMAGE:=
  DEPS:=
endef

define uboot/arv4519pw_ram
  TITLE:=U-Boot for Arcadyan arv4519pw (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/arcadyan/arv4519pw/ddr_settings.h
  DEPS:=@TARGET_lantiq_xway_ARV4519PW
endef

define uboot/arv4519pw_nor
  TITLE:=U-Boot for Arcadyan arv4519pw (NOR)
  SOC:=danube
  DEPS:=@TARGET_lantiq_xway_ARV4519PW
endef

define uboot/arv4519pw_brn
  TITLE:=U-Boot for Arcadyan arv4519pw (BRN)
  SOC:=danube
  DEPS:=@TARGET_lantiq_xway_ARV4519PW
endef

define uboot/arv7510pw_ram
  TITLE:=U-Boot for Arcadyan arv7510pw (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/arcadyan/arv7510pw/ddr_settings.h
  DEPS:=@TARGET_lantiq_xway_ARV4510PW
endef

define uboot/arv7510pw_nor
  TITLE:=U-Boot for Arcadyan arv7510pw (NOR)
  SOC:=danube
  DEPS:=@TARGET_lantiq_xway_ARV4510PW
endef

define uboot/arv7510pw_brn
  TITLE:=U-Boot for Arcadyan arv7510pw (BRN)
  SOC:=danube
  DEPS:=@TARGET_lantiq_xway_ARV4510PW
endef

define uboot/arv7518pw_ram
  TITLE:=U-Boot for Arcadyan arv7518pw (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/arcadyan/arv7518pw/ddr_settings.h
  DEPS:=@TARGET_lantiq_xway_ARV7518PW
endef

define uboot/arv7518pw_nor
  TITLE:=U-Boot for Arcadyan arv7518pw (NOR)
  SOC:=danube
  DEPS:=@TARGET_lantiq_xway_ARV7518PW
endef

define uboot/arv7518pw_brn
  TITLE:=U-Boot for Arcadyan arv7518pw (BRN)
  SOC:=danube
  DEPS:=@TARGET_lantiq_xway_ARV7518PW
endef

define uboot/arv752dpw_ram
  TITLE:=U-Boot for Arcadyan arv752dpw (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/arcadyan/arv752dpw/ddr_settings.h
  DEPS:=@TARGET_lantiq_xway_ARV752DPW
endef

define uboot/arv752dpw_nor
  TITLE:=U-Boot for Arcadyan arv752dpw (NOR)
  SOC:=danube
  DEPS:=@TARGET_lantiq_xway_ARV752DPW
endef

define uboot/arv752dpw_brn
  TITLE:=U-Boot for Arcadyan arv752dpw (BRN)
  SOC:=danube
  DEPS:=@TARGET_lantiq_xway_ARV752DPW
endef

define uboot/arv752dpw22_ram
  TITLE:=U-Boot for Arcadyan arv752dpw22 (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/arcadyan/arv752dpw22/ddr_settings.h
  DEPS:=@TARGET_lantiq_xway_ARV752DPW22
endef

define uboot/arv752dpw22_nor
  TITLE:=U-Boot for Arcadyan arv752dpw22 (NOR)
  SOC:=danube
  DEPS:=@TARGET_lantiq_xway_ARV752DPW22
endef

define uboot/arv752dpw22_brn
  TITLE:=U-Boot for Arcadyan arv752dpw22 (BRN)
  SOC:=danube
  DEPS:=@TARGET_lantiq_xway_ARV752DPW22
endef

define uboot/gigasx76x_ram
  TITLE:=U-Boot for Siemens Gigaset sx76x (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/gigaset/sx76x/ddr_settings.h
  DEPS:=@TARGET_lantiq_xway_GIGASX76X
endef

define uboot/gigasx76x_nor
  TITLE:=U-Boot for Siemens Gigaset sx76x (NOR)
  SOC:=danube
  DEPS:=@TARGET_lantiq_xway_GIGASX76X
endef

define uboot/acmp252_ram
  TITLE:=U-Boot for AudioCodes MP-252 (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/audiocodes/acmp252/ddr_settings.h
  DEPS:=@TARGET_lantiq_xway_ACMP252
endef

define uboot/acmp252_nor
  TITLE:=U-Boot for AudioCodes MP-252 (NOR)
  SOC:=danube
  DEPS:=@TARGET_lantiq_xway_ACMP252
endef

define uboot/easy50712_ram
  TITLE:=U-Boot for Lantiq EASY50712 (RAM)
  SOC:=danube
  DDR_SETTINGS:=board/lantiq/easy50712/ddr_settings.h
  DEPS:=@TARGET_lantiq_xway_EASY50712
endef

define uboot/easy50712_nor
  TITLE:=U-Boot for Lantiq EASY50712 (NOR)
  SOC:=danube
  DEPS:=@TARGET_lantiq_xway_EASY50712
endef

define uboot/easy50712_norspl
  TITLE:=U-Boot for Lantiq EASY50712 (NOR SPL)
  SOC:=danube
  IMAGE:=u-boot.ltq.lzo.norspl
  DEPS:=@TARGET_lantiq_xway_EASY50712
endef

define uboot/easy80920_ram
  TITLE:=U-Boot for Lantiq EASY80920 (RAM)
  SOC:=vr9
  DDR_SETTINGS:=board/lantiq/easy80920/ddr_settings.h
  DEPS:=@(TARGET_lantiq_xrx200_EASY80920NOR||TARGET_lantiq_xrx200_EASY80920NAND)
endef

define uboot/easy80920_nor
  TITLE:=U-Boot for Lantiq EASY80920 (NOR)
  SOC:=vr9
  DEPS:=@(TARGET_lantiq_xrx200_EASY80920NOR||TARGET_lantiq_xrx200_EASY80920NAND)
endef

define uboot/easy80920_norspl
  TITLE:=U-Boot for Lantiq EASY80920 (NOR SPL)
  SOC:=vr9
  IMAGE:=u-boot.ltq.lzo.norspl
  DEPS:=@(TARGET_lantiq_xrx200_EASY80920NOR||TARGET_lantiq_xrx200_EASY80920NAND)
endef

define uboot/easy80920_sfspl
  TITLE:=U-Boot for Lantiq EASY80920 (SPI SPL)
  SOC:=vr9
  IMAGE:=u-boot.ltq.lzo.sfspl
  DEPS:=@(TARGET_lantiq_xrx200_EASY80920NOR||TARGET_lantiq_xrx200_EASY80920NAND)
endef

define uboot/fb3370_eva
  TITLE:=U-Boot for AVM FRITZ3370 (EVA)
  SOC:=vr9
  DEPS:=@TARGET_lantiq_xrx200_FRITZ3370
endef

define uboot/fb3370_ram
  TITLE:=U-Boot for AVM FRITZ3370 (RAM)
  SOC:=vr9
  DDR_SETTINGS:=board/avm/fb3370/ddr_settings.h
  DEPS:=@TARGET_lantiq_xrx200_FRITZ3370
endef

define uboot/fb3370_sfspl
  TITLE:=U-Boot for AVM FRITZ3370 (SPI SPL)
  SOC:=vr9
  IMAGE:=u-boot.ltq.lzo.sfspl
  DEPS:=@TARGET_lantiq_xrx200_FRITZ3370
endef

define uboot/p2812hnufx_ram
  TITLE:=U-Boot for ZyXEL P-2812HNU-Fx (RAM)
  SOC:=vr9
  DDR_SETTINGS:=board/zyxel/p2812hnufx/ddr_settings.h
  DEPS:=@TARGET_lantiq_xrx200_P2812HNUFX
endef

define uboot/p2812hnufx_nandspl
  TITLE:=U-Boot for ZyXEL P-2812HNU-Fx (NAND SPL)
  SOC:=vr9
  IMAGE:=u-boot.ltq.lzo.nandspl
  DEPS:=@TARGET_lantiq_xrx200_P2812HNUFX
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
	easy80920_ram easy80920_nor easy80920_norspl easy80920_sfspl \
	fb3370_eva fb3370_ram fb3370_sfspl \
	p2812hnufx_ram p2812hnufx_nandspl

define Package/uboot/template
define Package/uboot-lantiq-$(1)
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=$(3)
  TITLE:=$(2)
  URL:=http://www.denx.de/wiki/U-Boot
  VARIANT:=$(1)
  MAINTAINER:=Luka Perkov <luka@openwrt.org>
endef
endef

define BuildUBootPackage
	$(eval $(uboot/Default))
	$(eval $(uboot/$(1)))
	DEPS:=$(uboot/$(1)/DEPS)
	$(call Package/uboot/template,$(1),$(TITLE),$(DEPS))
endef

define CopyVR9Firmware
	$(CP) $(FIRMWARE_LANTIQ_SOURCE)/vr9_phy$(1)_a$(2)x.bin \
		$(PKG_BUILD_DIR)/arch/mips/cpu/mips32/vrx200/fw_phy$(1)_a$(2)x.blob
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	mkdir -p $(PKG_BUILD_DIR)/arch/mips/cpu/mips32/vrx200/
	$(call CopyVR9Firmware,11g,1)
	$(call CopyVR9Firmware,11g,2)
	$(call CopyVR9Firmware,22f,1)
	$(call CopyVR9Firmware,22f,2)
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
