#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=grub
PKG_VERSION:=0.97
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://alpha.gnu.org/gnu/grub
PKG_MD5SUM:=cd3f3eb54446be6003156158d51f4884

PKG_BUILD_DEPENDS:= grub/host
PKG_INSTALL:=1

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

define Package/grub
  SUBMENU:=Boot Loaders
  CATEGORY:=Utilities
  SECTION:=utils
  TITLE:=GRand Unified Bootloader
  URL:=http://www.gnu.org/software/grub/
  DEPENDS:=@TARGET_x86
endef


MY_CONFIGURE_ARGS += \
	--disable-auto-linux-mem-opt \
	--disable-graphics \
	--disable-hercules \
	--without-curses \

MY_CONFIGURE_VARS += \
	grub_cv_prog_objcopy_absolute=yes \

CONFIGURE_ARGS += $(MY_CONFIGURE_ARGS)

CONFIGURE_VARS += $(MY_CONFIGURE_VARS)

HOST_CFLAGS += $(call host-cc-option,-fno-stack-protector)
HOST_CFLAGS += $(call host-cc-option,-U_FORTIFY_SOURCE)

HOST_CONFIGURE_ARGS += $(MY_CONFIGURE_ARGS) \
	--sbindir="$(STAGING_DIR_HOST)/bin" \

HOST_CONFIGURE_VARS += $(MY_CONFIGURE_VARS)

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/grub $(1)/usr/lib/
endef

define Package/grub/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/* $(1)/usr/sbin/
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,grub))
