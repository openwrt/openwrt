#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=fakeroot
PKG_VERSION:=1.24

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://deb.debian.org/debian/pool/main/f/fakeroot
PKG_HASH:=2e045b3160370b8ab4d44d1f8d267e5d1d555f1bb522d650e7167b09477266ed
PKG_LICENSE:=GPL-3.0-or-later
PKG_LICENSE_FILES:=COPYING

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_VARS += \
	ac_cv_header_sys_capability_h=no \
	ac_cv_func_capset=no

HOST_CONFIGURE_ARGS += \
	--with-ipc=tcp

$(eval $(call HostBuild))
