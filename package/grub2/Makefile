#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=grub
PKG_VERSION:=2.00
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/grub
PKG_MD5SUM:=e927540b6eda8b024fb0391eeaa4091c

PKG_HOST_ONLY:=1
HOST_BUILD_PARALLEL:=1
PKG_BUILD_DEPENDS:=grub2/host

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

define Package/grub2
  SUBMENU:=Boot Loaders
  CATEGORY:=Utilities
  SECTION:=utils
  TITLE:=GRand Unified Bootloader
  URL:=http://www.gnu.org/software/grub/
  DEPENDS:=@TARGET_x86
endef

HOST_CONFIGURE_ARGS += \
	--target=$(REAL_GNU_TARGET_NAME) \
	--sbindir="$(STAGING_DIR_HOST)/bin" \
	--disable-werror \
	--disable-nls

HOST_MAKE_FLAGS += \
	TARGET_RANLIB=$(TARGET_RANLIB) \
	LIBLZMA=$(STAGING_DIR_HOST)/lib/liblzma.a

define Host/Configure
	$(SED) 's,(RANLIB),(TARGET_RANLIB),' $(HOST_BUILD_DIR)/grub-core/Makefile.in
	$(Host/Configure/Default)
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,grub2))
