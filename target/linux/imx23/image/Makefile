#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/BuildKernel/olinuxino-bootlet
	cat $(LINUX_DIR)/arch/arm/boot/zImage $(LINUX_DIR)/arch/arm/boot/dts/imx23-olinuxino.dtb > $(STAGING_DIR)/zImage_dtb
	(cd $(STAGING_DIR); \
	$(STAGING_DIR)/../host/bin/elftosb -z -c ./linux_prebuilt.db -o linux.sb; \
	dd if=/dev/zero of=sd_mmc_bootstream.raw bs=512 count=4; \
	dd if=linux.sb of=$(BIN_DIR)/openwrt-imx23-sbImage ibs=512 seek=4; \
	)
endef

define Image/BuildKernel

  ifeq ($(CONFIG_PACKAGE_imx-bootlets),y)
	$(call Image/BuildKernel/olinuxino-bootlet)
  endif

endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
endef

$(eval $(call BuildImage))

