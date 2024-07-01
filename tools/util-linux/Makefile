#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=util-linux
PKG_VERSION:=2.40.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@KERNEL/linux/utils/$(PKG_NAME)/v2.40
PKG_HASH:=8e396eececae2b3b68db232c33b8810faa7c31f6df19f98f512739293d5829b7
PKG_CPE_ID:=cpe:/a:kernel:util-linux

PKG_FIXUP:=autoreconf

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS += \
	--with-pic \
	--disable-shared \
	--disable-all-programs \
	--enable-hexdump \
	--enable-libuuid \
	--without-util \
	--without-selinux \
	--without-audit \
	--without-udev \
	--without-ncursesw \
	--without-ncurses \
	--without-slang \
	--without-tinfo \
	--without-readline \
	--without-utempter \
	--without-cap-ng \
	--without-libz \
	--without-libmagic \
	--without-user \
	--without-btrfs \
	--without-systemd \
	--without-smack \
	--without-econf \
	--without-python \
	--without-cryptsetup

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
