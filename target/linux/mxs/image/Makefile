#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

BOARDS:= \
	imx23-olinuxino

define Image/BuildKernel/olinuxino-bootlet
	cat $(LINUX_DIR)/arch/arm/boot/zImage $(LINUX_DIR)/arch/arm/boot/dts/imx23-olinuxino.dtb > $(STAGING_DIR)/zImage_dtb
	(cd $(STAGING_DIR); \
	$(STAGING_DIR)/../host/bin/elftosb -z -c ./linux_prebuilt.db -o linux.sb; \
	dd if=/dev/zero of=sd_mmc_bootstream.raw bs=512 count=4; \
	dd if=linux.sb of=$(BIN_DIR)/openwrt-imx23-sbImage ibs=512 seek=4; \
	)
endef

define Image/BuildKernel/olinuxino-uboot
	mkimage -A arm -O linux -T kernel -C none \
		-a 0x42000000 -e 0x42000000 \
		-n 'ARM OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux $(KDIR)/uImage
	cp $(KDIR)/uImage $(BIN_DIR)/$(IMG_PREFIX)-uImage
endef

define Image/BuildKernel

  ifeq ($(CONFIG_PACKAGE_imx-bootlets),y)
	$(call Image/BuildKernel/olinuxino-bootlet)
  endif

  ifeq ($(CONFIG_PACKAGE_uboot-imx23-mx23_olinuxino),y)
	$(call Image/BuildKernel/olinuxino-uboot)
  endif
  
	$(CP) $(LINUX_DIR)/arch/arm/boot/zImage $(BIN_DIR)/openwrt-$(BOARD)-zImage
	$(foreach board,$(BOARDS),
		$(CP) $(LINUX_DIR)/arch/arm/boot/dts/$(board).dtb $(BIN_DIR)/
	)
endef

define Image/InstallKernel

  ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_KERNEL),)
	mkdir -p $(TARGET_DIR)/boot
	$(CP) $(LINUX_DIR)/arch/arm/boot/zImage $(TARGET_DIR)/boot/
  endif
  
  ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_DTB),)
	mkdir -p $(TARGET_DIR)/boot
	$(foreach board,$(BOARDS),
		$(CP) $(LINUX_DIR)/arch/arm/boot/dts/$(board).dtb $(TARGET_DIR)/boot/
	)
  endif

endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
endef

$(eval $(call BuildImage))
