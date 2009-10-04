#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=u-boot
PKG_VERSION:=1.1.5
PKG_RELEASE:=2

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.denx.de/pub/u-boot
PKG_MD5SUM:=579707c8ecbf1ab4127285d2aac2a9ee
PKG_TARGETS:=bin

include $(INCLUDE_DIR)/package.mk

define Package/uboot-ifxmips
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_ifxmips
  TITLE:=U-Boot for Infineon MIPS boards
  URL:=http://www.denx.de/wiki/UBoot/WebHome
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	cp -r $(CP_OPTS) ./files/* $(PKG_BUILD_DIR)
	find $(PKG_BUILD_DIR) -name .svn | $(XARGS) rm -rf
endef

UBOOT_CONFIG:=danube

UBOOT_MAKE_OPTS:=\
	CROSS_COMPILE=$(TARGET_CROSS) \
	CROSS_COMPILE_UCLIBC=1 \
	COMPRESS=lzma \
	PLATFORM_CPU=mips32r2 \
	UBOOT_RAM_TEXT_BASE=0xA0400000 \

define Build/Configure
	$(MAKE) -s -C $(PKG_BUILD_DIR) \
		$(UBOOT_MAKE_OPTS) \
		$(UBOOT_CONFIG)_config
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(UBOOT_MAKE_OPTS) \
		OWRT_FLAGS="-DTEXT_BASE=0xa0400000" \
		ifx_all
	$(CP) $(PKG_BUILD_DIR)/u-boot.srec $(PKG_BUILD_DIR)/asc.srec
	$(PKG_BUILD_DIR)/gct \
		$(PKG_BUILD_DIR)/danube_ref_ddr166.conf \
		$(PKG_BUILD_DIR)/asc.srec \
		$(PKG_BUILD_DIR)/u-boot.asc
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(UBOOT_MAKE_OPTS) \
		OWRT_FLAGS="-DDANUBE_BOOT_FROM_EBU=1 -DTEXT_BASE=0xB0000000" \
		clean ifx_all
endef

define Package/uboot-ifxmips/install
	mkdir -p $(1)
	dd if=$(PKG_BUILD_DIR)/u-boot.ifx of=$(1)/u-boot.ifx bs=64k conv=sync
	$(CP) $(PKG_BUILD_DIR)/u-boot.asc $(1)
endef

$(eval $(call BuildPackage,uboot-ifxmips))
