#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=goldfish-qemu
PKG_REV:=2b8ea29e2bd12f876a4d06647e6077bf72de567e
PKG_VERSION:=20090429
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://android.git.kernel.org/platform/external/qemu
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_TARGETS:=bin

include $(INCLUDE_DIR)/package.mk

define Package/goldfish-qemu
  SECTION:=emulator
  CATEGORY:=Emulators
  DEPENDS:=@TARGET_goldfish
  TITLE:=A modified version of the Google Android Emulator
  URL:=http://www.android.com/
endef

LIBSDL_PATCHED:=sdl-1.2.12-android-20080919

define Download/libsdl-patched
  FILE:=$(LIBSDL_PATCHED).tar.gz
  URL:=http://android.git.kernel.org/pub
  MD5SUM:=22df8cbb2ecb811938eba8410e861650
endef
$(eval $(call Download,libsdl-patched))

Build/Exports=

define Build/Prepare
	$(call Build/Prepare/Default)
	zcat $(DL_DIR)/$(LIBSDL_PATCHED).tar.gz | tar x -C $(PKG_BUILD_DIR)
endef

define Build/Configure
	[ -x $(PKG_BUILD_DIR)/libsdl/bin/sdl-config ] || ( \
		cd $(PKG_BUILD_DIR)/$(LIBSDL_PATCHED); \
		./android-configure --prefix=$(PKG_BUILD_DIR)/libsdl; \
		make all install; \
	)
endef

define Build/Compile
	(cd $(PKG_BUILD_DIR); \
		[ -f $(PKG_BUILD_DIR)/objs/config.make ] || \
			./android-configure.sh --sdl-config=$(PKG_BUILD_DIR)/libsdl/bin/sdl-config \
	)
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/goldfish-qemu/install
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_BUILD_DIR)/objs/emulator $(1)/
	$(CP) ./skins $(1)/
endef

$(eval $(call BuildPackage,goldfish-qemu))
