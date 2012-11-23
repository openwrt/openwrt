#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=u-boot
PKG_VERSION:=2012.07
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://dev.phrozen.org/uboot-upstream.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=a0342fc87a884a2f60e2849bcd48fe366ccf9366
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_TARGETS:=bin

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define uboot/Default
  TITLE:=
  CONFIG:=
  IMAGE:=
endef

define uboot/arv4519pw_ram
  TITLE:=U-Boot for Arcadyan arv4519pw (RAM)
  DDR_SETTINGS:=board/arcadyan/arv4519pw/ddr_settings.h
endef

define uboot/arv4519pw_nor
  TITLE:=U-Boot for Arcadyan arv4519pw (NOR)
endef

define uboot/arv7518pw_ram
  TITLE:=U-Boot for Arcadyan arv7518pw (RAM)
  DDR_SETTINGS:=board/arcadyan/arv7518pw/ddr_settings.h
endef

define uboot/arv7518pw_nor
  TITLE:=U-Boot for Arcadyan arv7518pw (NOR)
endef

define uboot/gigasx76x_ram
  TITLE:=U-Boot for Siemens Gigaset sx76x (RAM)
  DDR_SETTINGS:=board/gigaset/sx76x/ddr_settings.h
endef

define uboot/gigasx76x_nor
  TITLE:=U-Boot for Siemens Gigaset sx76x (NOR)
endef

UBOOTS:= \
	arv4519pw_ram arv4519pw_nor \
	arv7518pw_ram arv7518pw_nor \
	gigasx76x_ram gigasx76x_nor

define Package/uboot/template
define Package/uboot-lantiq-$(1)
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_lantiq
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

$(eval $(call uboot/$(BUILD_VARIANT)))

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(BUILD_VARIANT) CROSS_COMPILE=$(TARGET_CROSS)
ifneq ($(DDR_SETTINGS),)
	awk -f $(PKG_BUILD_DIR)/tools/lantiq_ram_init_uart.awk $(PKG_BUILD_DIR)/$(DDR_SETTINGS) > $(PKG_BUILD_DIR)/$(BUILD_VARIANT)_ddr_settings
	perl $(PKG_BUILD_DIR)/tools/gct.pl $(PKG_BUILD_DIR)/$(BUILD_VARIANT)_ddr_settings $(PKG_BUILD_DIR)/u-boot.srec  $(PKG_BUILD_DIR)/u-boot.asc
endif
endef

define Package/uboot/install/default
	$(INSTALL_DIR) $(BIN_DIR)/uboot-$(BOARD)-$(1)
	$(CP) $(PKG_BUILD_DIR)/u-boot.bin \
		$(BIN_DIR)/uboot-$(BOARD)-$(1)/openwrt-$(BOARD)-$(1)-u-boot.bin
ifneq ($(DDR_SETTINGS),)
	$(CP) $(PKG_BUILD_DIR)/u-boot.asc \
		$(BIN_DIR)/uboot-$(BOARD)-$(1)/openwrt-$(BOARD)-$(1)-u-boot.asc
endif
endef

define Package/uboot/install/template
define Package/uboot-lantiq-$(1)/install
	$(call Package/uboot/install/default,$(2))
endef
endef

$(foreach u,$(UBOOTS), \
	$(eval $(call Package/uboot/install/template,$(u),$(u))) \
)

$(foreach u,$(UBOOTS), \
	$(eval $(call BuildUBootPackage,$(u))) \
	$(eval $(call BuildPackage,uboot-lantiq-$(u))) \
)
