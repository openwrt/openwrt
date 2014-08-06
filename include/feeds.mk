#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

-include $(TMP_DIR)/.packagefeeds

FEEDS_AVAILABLE:=$(shell $(SCRIPT_DIR)/feeds list -n)
FEEDS_INSTALLED:=$(notdir $(wildcard $(TOPDIR)/package/feeds/*))
FEEDS_ENABLED:=$(foreach feed,$(FEEDS_INSTALLED),$(if $(CONFIG_FEED_$(feed)),$(feed)))
FEEDS_DISABLED:=$(filter-out $(FEEDS_ENABLED),$(FEEDS_INSTALLED))

PKG_CONFIG_DEPENDS += \
	CONFIG_PER_FEED_REPO \
	$(foreach feed,$(FEEDS_INSTALLED),CONFIG_FEED_$(feed))

# 1: package name
define FeedPackageDir
$(strip $(if $(CONFIG_PER_FEED_REPO), \
  $(abspath $(PACKAGE_DIR)/$(if $(Package/$(1)/feed),$(Package/$(1)/feed),base)), \
  $(PACKAGE_DIR)))
endef
