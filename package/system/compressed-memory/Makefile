#
# Copyright (C) 2019-2020 Oever González <notengobattery@gmail.com>
#
# Licensed to the public under the Apache License 2.0.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=compressed-memory
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/compressed-memory
  SECTION:=utils
  CATEGORY:=Base system
  DEPENDS:= \
	+kmod-crypto-deflate \
	+kmod-crypto-zstd \
	+kmod-lib-lz4 \
	+kmod-lib-lz4hc \
	+kmod-lib-lzo \
	+!(BUSYBOX_DEFAULT_MKSWAP||BUSYBOX_CONFIG_MKSWAP):swap-utils \
	+!((BUSYBOX_DEFAULT_SWAPON||BUSYBOX_CONFIG_SWAPON)&&(BUSYBOX_DEFAULT_SWAPOFF||BUSYBOX_CONFIG_SWAPOFF)):block-mount
  HIDDEN:=1
  TITLE:=Configuration files for compressed-memory
  MAINTAINER:=Oever Gonzalez <notengobattery@gmail.com>
  PKGARCH:=all
endef

define Package/compressed-memory/description
 Installs a UCI file which contains pre-computed values for packages that belong
 to compressed-memory.
endef

define Package/swap-zram-writeback
  SECTION:=utils
  CATEGORY:=Base system
  DEPENDS:= +compressed-memory \
	+kmod-zram-writeback \
	+losetup
  TITLE:=Script to manage compressed memory for swap
  MAINTAINER:=Oever Gonzalez <notengobattery@gmail.com>
  PKGARCH:=all
endef

define Package/swap-zram-writeback/description
 A script to activate compressed memory for a swap device. This can increase
 the total available memory by compressing the memory that is not accessed
 often (by using an algorith called LRU).

 Most programs allocate memory that they do not use after the startup.
 Allowing the system to compress that memory reduces the memory pressure as a
 trade-off for CPU cycles.
endef

define Package/swap-zswap-cache
  SECTION:=utils
  CATEGORY:=Base system
  DEPENDS:= +compressed-memory \
	+kmod-zswap
  TITLE:=Script to manage compressed cache for swap
  MAINTAINER:=Oever Gonzalez <notengobattery@gmail.com>
  PKGARCH:=all
endef

define Package/swap-zswap-cache/description
 A script to activate compressed memory cache. This compressed cache will be
 depleted before it begins to evict memory pages to a slow (real) swap device.

 This can save good amounts of I/O to the swap device at the cost of CPU cycles
 and a percentage of the memory. As compressing memory may be cheaper than
 performing I/O, the system may run smoother under memory pressure.

 If the pool is depleted and the system begins to starve, it will freeze as it
 does when swapping normally. The point of this package is to delay the point
 where it happens.
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/compressed-memory/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/compressed_memory.uci $(1)/etc/config/compressed_memory
endef

define Package/swap-zram-writeback/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/zram.init $(1)/etc/init.d/zram
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) ./files/zram.script $(1)/bin/zram
endef

define Package/swap-zswap-cache/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/zswap.init $(1)/etc/init.d/zswap
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) ./files/zswap.script $(1)/bin/zswap
endef

$(eval $(call BuildPackage,compressed-memory))
$(eval $(call BuildPackage,swap-zram-writeback))
$(eval $(call BuildPackage,swap-zswap-cache))
