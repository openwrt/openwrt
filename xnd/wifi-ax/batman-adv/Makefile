# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=batman-adv
PKG_VERSION:=2021.1
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://downloads.open-mesh.org/batman/releases/batman-adv-$(PKG_VERSION)
PKG_HASH:=bf77843d8dead75342d673ce7021e4ad037447ce18c64056ae1e3202039934d0
PKG_EXTMOD_SUBDIRS:=net/batman-adv

PKG_MAINTAINER:=Simon Wunderlich <sw@simonwunderlich.de>
PKG_LICENSE:=GPL-2.0-only MIT
PKG_LICENSE_FILES:=LICENSES/preferred/GPL-2.0 LICENSES/preferred/MIT

STAMP_CONFIGURED_DEPENDS := $(STAGING_DIR)/usr/include/mac80211-backport/backport/autoconf.h

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/batman-adv
  SUBMENU:=Network Support
  TITLE:=B.A.T.M.A.N. Adv
  URL:=https://www.open-mesh.org/
  DEPENDS:=+BATMAN_ADV_BLA:kmod-lib-crc16 +kmod-lib-crc32c +kmod-cfg80211 +batctl
  FILES:=$(PKG_BUILD_DIR)/net/batman-adv/batman-adv.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoProbe,batman-adv)
endef

define KernelPackage/batman-adv/description
  B.A.T.M.A.N. (better approach to mobile ad-hoc networking) is
  a routing protocol for multi-hop ad-hoc mesh networks. The
  networks may be wired or wireless. See
  https://www.open-mesh.org/ for more information and user space
  tools. This package builds version $(PKG_VERSION) of the kernel
  module.
endef

define KernelPackage/batman-adv/config
	source "$(SOURCE)/Config.in"
endef

define Package/kmod-batman-adv/conffiles
/etc/config/batman-adv
endef

PKG_EXTRA_KCONFIG:= \
	CONFIG_BATMAN_ADV=m \
	CONFIG_BATMAN_ADV_DEBUG=$(if $(CONFIG_BATMAN_ADV_DEBUG),y,n) \
	CONFIG_BATMAN_ADV_BLA=$(if $(CONFIG_BATMAN_ADV_BLA),y,n) \
	CONFIG_BATMAN_ADV_DAT=$(if $(CONFIG_BATMAN_ADV_DAT),y,n) \
	CONFIG_BATMAN_ADV_MCAST=$(if $(CONFIG_BATMAN_ADV_MCAST),y,n) \
	CONFIG_BATMAN_ADV_NC=$(if $(CONFIG_BATMAN_ADV_NC),y,n) \
	CONFIG_BATMAN_ADV_BATMAN_V=$(if $(CONFIG_BATMAN_ADV_BATMAN_V),y,n) \
	CONFIG_BATMAN_ADV_TRACING=$(if $(CONFIG_BATMAN_ADV_TRACING),y,n) \

PKG_EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(PKG_EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(PKG_EXTRA_KCONFIG)))) \

NOSTDINC_FLAGS = \
	-I$(PKG_BUILD_DIR)/net/batman-adv \
	-I$(STAGING_DIR)/usr/include/mac80211-backport \
	-I$(STAGING_DIR)/usr/include/mac80211-backport/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211 \
	-I$(STAGING_DIR)/usr/include/mac80211/uapi \
	-I$(PKG_BUILD_DIR)/include/ \
	-include backport/autoconf.h \
	-include backport/backport.h \
	-include $(PKG_BUILD_DIR)/compat-hacks.h \
	-include $(PKG_BUILD_DIR)/compat-qca.h \
	-DBATADV_SOURCE_VERSION=\\\"$(PKG_VERSION)-openwrt-$(PKG_RELEASE)\\\"

define Build/Compile
	$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		$(KERNEL_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)/net/batman-adv" \
		$(PKG_EXTRA_KCONFIG) \
		EXTRA_CFLAGS="$(PKG_EXTRA_CFLAGS)" \
		NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
		modules
endef

define KernelPackage/batman-adv/install
	$(CP) ./files/. $(1)/
endef

$(eval $(call KernelPackage,batman-adv))
