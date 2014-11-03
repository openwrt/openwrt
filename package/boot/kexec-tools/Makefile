#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=kexec-tools
PKG_VERSION:=2.0.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@KERNEL/linux/utils/kernel/kexec
PKG_MD5SUM:=05992bc8c0673fc55be7b6d27e48a8db

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=COPYING

PKG_FIXUP:=autoreconf

PKG_CONFIG_DEPENDS := CONFIG_KEXEC_ZLIB

include $(INCLUDE_DIR)/package.mk

define Package/kexec-tools
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@armeb||@arm||@i386||@powerpc64||@mipsel||@mips +KEXEC_ZLIB:zlib
  TITLE:=Kernel boots kernel
  URL:=http://kernel.org/pub/linux/kernel/people/horms/kexec-tools/
  MAINTAINER:=Florian Fainelli <florian@openwrt.org>
  MENU:=1
endef

define Package/kexec-tools/description
 kexec is a set of systems call that allows you to load
 another kernel from the currently executing Linux kernel.
endef

define Package/kexec-tools/config
	source "$(SOURCE)/Config.in"
endef

KEXEC_TARGET_NAME:=$(call qstrip,$(CONFIG_KEXEC_TOOLS_TARGET_NAME))-linux-$(TARGET_SUFFIX)

CONFIGURE_ARGS = \
		--target=$(KEXEC_TARGET_NAME) \
		--host=$(REAL_GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		$(if $(CONFIG_KEXEC_ZLIB),--with,--without)-zlib \
		--without-lzma

TARGET_CFLAGS += -ffunction-sections -fdata-sections
TARGET_LDFLAGS += -Wl,--gc-sections

CONFIGURE_VARS += \
	BUILD_CC="$(HOSTCC)" \
	TARGET_CC="$(TARGET_CC)"

kexec-extra-sbin-$(CONFIG_KEXEC_TOOLS_kdump) += kdump

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) DESTDIR="$(PKG_INSTALL_DIR)" all install
endef

define Package/kexec-tools/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) \
		$(addprefix $(PKG_INSTALL_DIR)/usr/sbin/, \
			$(kexec-extra-sbin-y)) \
		$(kexec-extra-bin-y) \
		$(PKG_INSTALL_DIR)/usr/sbin/kexec \
		$(1)/usr/sbin

# make a link for compatability with other distros
	$(INSTALL_DIR) $(1)/sbin
	ln -s /usr/sbin/kexec $(1)/sbin/kexec
endef

$(eval $(call BuildPackage,kexec-tools))
