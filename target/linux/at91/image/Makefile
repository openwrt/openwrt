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
	mkdir -p $(BIN_DIR)
	mkimage -A arm -T kernel -C none -a 0x20008000 -e 0x20008000 -n linux-2.6 \
		-d $(LINUX_DIR)/arch/arm/boot/Image $(BIN_DIR)/$(IMG_PREFIX)-uImage
	if [ $(CONFIG_FLEXIBITY_ROOT) ]; then \
		$(INSTALL_BIN) $(BIN_DIR)/$(IMG_PREFIX)-uImage $(TARGET_DIR)/uImage ; \
	fi
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	mkdir -p $(BIN_DIR)
	cp $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-root.$(1)
endef

$(eval $(call BuildImage))
