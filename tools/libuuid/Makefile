#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=util-linux-ng
PKG_VERSION:=2.18
PKG_MD5SUM:=aff22adfff1bf39cd3f13371b95a3d3a
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.kernel.org/pub/linux/utils/util-linux-ng/v2.18/

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
	$(call Host/Configure/Default, \
		--disable-shared \
		--enable-static \
		--disable-rpath \
		--disable-tls \
		--disable-mount \
		--disable-fsck \
		--enable-libuuid \
		--disable-uuidd \
		--disable-libblkid \
		--disable-libmount \
		--disable-nls \
		--disable-rpath \
		--disable-arch \
		--disable-agetty \
		--disable-cramfs \
		--disable-switch \
		--disable-pivot \
		--disable-fallocate \
		--disable-unshare \
		--disable-elvtune \
		--disable-init \
		--disable-kill \
		--disable-last \
		--disable-mesg \
		--disable-partx \
		--disable-raw \
		--disable-rename \
		--disable-reset \
		--disable-login-utils \
		--disable-schedutils \
		--disable-wall \
		--disable-write \
		--disable-chsh-only-listed \
		--disable-login-chown-vcs \
		--disable-login-stat-mail \
		--disable-pg-bell \
		--disable-require-password \
		--disable-fs-paths-default \
		--disable-fs-paths-extra \
		--disable-use-tty-group \
		--disable-makeinstall-chown \
		--disable-makeinstall-setuid \
		--without-libiconv-prefix \
		--without-libintl-prefix \
		--without-ncurses \
		--without-slang \
		--without-utempter \
		--without-pam \
		--without-selinux \
		--without-audit \
	)
endef

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR)/shlibs/uuid/src
endef

define Host/Install
	$(MAKE) -C $(HOST_BUILD_DIR)/shlibs/uuid/src install
endef

define Host/Clean
	-$(MAKE) -C $(HOST_BUILD_DIR)/shlibs/uuid/src uninstall
endef

$(eval $(call HostBuild))
