#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=u-boot
PKG_VERSION:=2009.11.1
PKG_MD5SUM:=6086421c9e2f3a0d0dbc5f706b551dbc
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.denx.de/pub/u-boot
PKG_TARGETS:=bin

include $(INCLUDE_DIR)/package.mk

define Package/uboot-lantiq
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_ifxmips
  TITLE:=U-Boot for Lantiq reference boards
  URL:=http://www.denx.de/wiki/U-Boot
endef

define Build/Prepare
	$(PKG_UNPACK)
	cp -r $(CP_OPTS) $(FILES_DIR)/* $(PKG_BUILD_DIR)/
	$(Build/Patch)
	find $(PKG_BUILD_DIR) -name .svn | $(XARGS) rm -rf
endef

UBOOT_CONFIG:=$(call qstrip,$(CONFIG_UBOOT_CONFIG))
UBOOT_DDR_CONFIG:=$(call qstrip,$(CONFIG_UBOOT_DDR_CONFIG))

UBOOT_MAKE_OPTS:= \
	CROSS_COMPILE=$(TARGET_CROSS) \
	ENDIANNESS= \
	V=1

define Package/uboot-lantiq/config
	menu "configuration"
		source "$(SOURCE)/Config.in"
	endmenu
endef

define Build/Configure/Target
	$(MAKE) -s -C $(PKG_BUILD_DIR) \
		$(UBOOT_MAKE_OPTS) \
		O=$(PKG_BUILD_DIR)/$(1) \
		$(1)_config
endef

define Build/Configure
	$(call Build/Configure/Target,$(UBOOT_CONFIG))
ifeq ($(CONFIG_UBOOT_RAMBOOT),y)
	$(call Build/Configure/Target,$(UBOOT_CONFIG)_ramboot)
endif
endef

define Build/Compile/Target
	$(MAKE) -s -C $(PKG_BUILD_DIR) \
		$(UBOOT_MAKE_OPTS) \
		O=$(PKG_BUILD_DIR)/$(1) \
		all
endef

define Build/Compile
	$(call Build/Compile/Target,$(UBOOT_CONFIG))
ifeq ($(CONFIG_UBOOT_RAMBOOT),y)
	$(call Build/Compile/Target,$(UBOOT_CONFIG)_ramboot)
endif
endef

define Package/uboot-lantiq/install
	mkdir -p $(1)/$(UBOOT_CONFIG)
	dd \
		if=$(PKG_BUILD_DIR)/$(UBOOT_CONFIG)/u-boot.bin \
		of=$(1)/$(UBOOT_CONFIG)/u-boot.bin \
		bs=64k conv=sync
ifeq ($(CONFIG_UBOOT_RAMBOOT),y)
	if [ -e $(UBOOT_DDR_CONFIG) ]; then \
		perl ./gct \
			$(UBOOT_DDR_CONFIG) \
			$(PKG_BUILD_DIR)/$(UBOOT_CONFIG)_ramboot/u-boot.srec \
			$(1)/$(UBOOT_CONFIG)/u-boot.asc; \
	fi
endif
endef

$(eval $(call BuildPackage,uboot-lantiq))
