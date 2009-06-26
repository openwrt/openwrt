# 
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ksymoops
PKG_VERSION:=2.4.11
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@KERNEL/linux/utils/kernel/ksymoops/v2.4/
PKG_MD5SUM:=4a8249e182a5dbc75e566d162e9f3314

include $(INCLUDE_DIR)/package.mk

define Package/ksymoops
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@LINUX_2_6 @mips||mipsel||m68k||i386 +libbfd
  TITLE:=Interpret oops of the kernel
  URL:=http://www.kernel.org/pub/linux/utils/kernel/ksymoops/v2.4/
  MENU:=1
endef

define Package/kexec-tools/description
endef

define Build/Configure
endef

TARGET=$(strip $(shell echo $(TARGET_CFLAGS) | cut -d- -f4 | cut -d= -f2))

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		BFD_PREFIX="$(STAGING_DIR)/usr" \
		DEF_TARGET='\"$(TARGET)\"' \
		DEF_ARCH='\"$(ARCH)\"' \
		DEF_MAP='\"$(LINUX_DIR)/System.map\"' \
		CROSS="$(TARGET_CROSS)" \
		INSTALL_PREFIX="$(PKG_INSTALL_DIR)" \
		all install
endef

define Package/ksymoops/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/$(TARGET_CROSS)ksymoops $(1)/bin/ksymoops
endef

$(eval $(call BuildPackage,ksymoops))
