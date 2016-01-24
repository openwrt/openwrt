#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

-include $(TMP_DIR)/.packagesubdirs

FEEDS_AVAILABLE:=$(shell $(SCRIPT_DIR)/feeds list -n)
FEEDS_INSTALLED:=$(notdir $(wildcard $(TOPDIR)/package/feeds/*))
FEEDS_ENABLED:=$(foreach feed,$(FEEDS_INSTALLED),$(if $(CONFIG_FEED_$(feed)),$(feed)))
FEEDS_DISABLED:=$(filter-out $(FEEDS_ENABLED),$(FEEDS_AVAILABLE))

PKG_CONFIG_DEPENDS += \
	CONFIG_PER_FEED_REPO \
	CONFIG_PER_FEED_REPO_ADD_DISABLED \
	CONFIG_PER_FEED_REPO_ADD_COMMENTED \
	$(foreach feed,$(FEEDS_INSTALLED),CONFIG_FEED_$(feed))

# 1: package name
define FeedPackageDir
$(strip $(if $(CONFIG_PER_FEED_REPO), \
  $(abspath $(PACKAGE_DIR)/$(if $(Package/$(1)/subdir),$(Package/$(1)/subdir),base)), \
  $(PACKAGE_DIR)))
endef

# 1: destination file
define FeedSourcesAppend
( \
  $(strip $(if $(CONFIG_PER_FEED_REPO), \
	$(foreach feed,base kernel $(FEEDS_ENABLED),echo "src/gz %n_$(feed) %U/$(feed)";) \
	$(if $(CONFIG_PER_FEED_REPO_ADD_DISABLED), \
		$(foreach feed,$(FEEDS_DISABLED),echo "$(if $(CONFIG_PER_FEED_REPO_ADD_COMMENTED),# )src/gz %n_$(feed) %U/$(feed)";)) \
  , \
	echo "src/gz %n %U"; \
  )) \
) >> $(1)
endef
