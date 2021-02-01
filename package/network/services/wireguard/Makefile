#
# Copyright (C) 2016-2019 Jason A. Donenfeld <Jason@zx2c4.com>
# Copyright (C) 2016 Baptiste Jonglez <openwrt@bitsofnetworks.org>
# Copyright (C) 2016-2017 Dan Luedtke <mail@danrl.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# When Linux 5.4 support is removed, kmod-wireguard should be moved to
# package/kernel/linux/modules/netsupport.mk and this package removed.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=wireguard

PKG_CONFIG_DEPENDS:=CONFIG_LINUX_5_10

PKG_VERSION-:=1.0.20201112
PKG_VERSION:=$(PKG_VERSION-$(CONFIG_LINUX_5_10))
PKG_RELEASE-:=1
PKG_RELEASE:=$(PKG_RELEASE-$(CONFIG_LINUX_5_10))


PKG_SOURCE-:=wireguard-linux-compat-$(PKG_VERSION).tar.xz
PKG_SOURCE:=$(PKG_SOURCE-$(CONFIG_LINUX_5_10))
PKG_SOURCE_URL-:=https://git.zx2c4.com/wireguard-linux-compat/snapshot/
PKG_SOURCE_URL:=$(PKG_SOURCE_URL-$(CONFIG_LINUX_5_10))
PKG_HASH-:=89eae7f0c0bd6c8df3ba2e090984974ff68741a9f26aa0922890f8ca727897e1
PKG_HASH:=$(PKG_HASH-$(CONFIG_LINUX_5_10))

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=COPYING

WG_VERSION-:=$(PKG_VERSION)
WG_VERSION-y:=$(LINUX_VERSION)
WG_VERSION:=$(WG_VERSION-$(CONFIG_LINUX_5_10))
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/wireguard-linux-compat-$(WG_VERSION)
PKG_BUILD_PARALLEL:=1
PKG_USE_MIPS16:=0

WG_BUILD_DIR-="$(PKG_BUILD_DIR)/src"
WG_BUILD_DIR-y="$(LINUX_DIR)/drivers/net/wireguard"

# WireGuard's makefile needs this to know where to build the kernel module
export KERNELDIR:=$(LINUX_DIR)

include $(INCLUDE_DIR)/package.mk

define Package/wireguard/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=VPN
  URL:=https://www.wireguard.com
  MAINTAINER:=Jason A. Donenfeld <Jason@zx2c4.com>
endef

define Package/wireguard/Default/description
  WireGuard is a novel VPN that runs inside the Linux Kernel and utilizes
  state-of-the-art cryptography. It aims to be faster, simpler, leaner, and
  more useful than IPSec, while avoiding the massive headache. It intends to
  be considerably more performant than OpenVPN.  WireGuard is designed as a
  general purpose VPN for running on embedded interfaces and super computers
  alike, fit for many different circumstances. It uses UDP.
endef

define Package/wireguard
  $(call Package/wireguard/Default)
  TITLE:=WireGuard meta-package
  VERSION:=$(WG_VERSION)
  DEPENDS:=+wireguard-tools +kmod-wireguard
endef

include $(INCLUDE_DIR)/kernel-defaults.mk
include $(INCLUDE_DIR)/package-defaults.mk

define Build/Compile
	$(MAKE) $(KERNEL_MAKEOPTS) M=$(WG_BUILD_DIR-$(CONFIG_LINUX_5_10)) modules
endef

define Package/wireguard/install
  true
endef

define Package/wireguard/description
  $(call Package/wireguard/Default/description)
endef

define KernelPackage/wireguard
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Support
  TITLE:=WireGuard kernel module
  KCONFIG:= \
	CONFIG_WIREGUARD \
	CONFIG_WIREGUARD_DEBUG=n
  DEPENDS:= \
	+IPV6:kmod-udptunnel6 \
	+kmod-udptunnel4 \
	+@!LINUX_5_4:kmod-crypto-curve25519 \
	+@!LINUX_5_4:kmod-crypto-chacha20poly1305 \
	+@!LINUX_5_4:kmod-crypto-blake2s \

  FILES:=$(WG_BUILD_DIR-$(CONFIG_LINUX_5_10))/wireguard.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoProbe,wireguard)
endef

define KernelPackage/wireguard/description
  $(call Package/wireguard/Default/description)

  This package provides the kernel module for WireGuard.
endef

$(eval $(call BuildPackage,wireguard))
$(eval $(call KernelPackage,wireguard))
