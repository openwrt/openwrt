# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/Clean
	$(MAKE) -C u-boot clean
endef

define Build/Compile
	if [ $(CONFIG_AT91_UBOOT) ]; then \
		$(MAKE) -C u-boot compile; \
	fi
endef

define Image/Prepare
endef

define Image/BuildKernel
	cp $(LINUX_DIR)/arch/arm/boot/uImage $(BIN_DIR)/$(IMG_PREFIX)-uImage
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))
