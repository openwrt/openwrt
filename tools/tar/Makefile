#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=tar
PKG_CPE_ID:=cpe:/a:gnu:tar
PKG_VERSION:=1.34

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/tar
PKG_HASH:=03d908cf5768cfe6b7ad588c921c6ed21acabfb2b79b788d1330453507647aed

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

HOSTCC := $(HOSTCC_NOCACHE)
HOSTCXX := $(HOSTCXX_NOCACHE)

HOST_CONFIGURE_ARGS += \
	--without-posix-acls \
	--without-selinux \
	--without-xattrs \
	--disable-acl \
	--disable-nls

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
