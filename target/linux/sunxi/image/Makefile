#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

BOARDS:= \
	sun4i-a10-cubieboard \
	sun4i-a10-olinuxino-lime \
	sun4i-a10-pcduino \
	sun5i-a13-olinuxino \
	sun6i-a31-colombus \
	sun7i-a20-cubieboard2 \
	sun7i-a20-cubietruck \
	sun7i-a20-olinuxino-micro

define Image/BuildKernel
	mkimage -A arm -O linux -T kernel -C none \
		-a 0x40008000 -e 0x40008000 \
		-n 'ARM OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/zImage $(BIN_DIR)/$(IMG_PREFIX)-uImage
	
    ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(CP) $(KDIR)/zImage-initramfs $(BIN_DIR)/$(IMG_PREFIX)-zImage-initramfs
	echo -ne '\x00\x00\x00\x00' >> $(BIN_DIR)/$(IMG_PREFIX)-zImage-initramfs
	$(call Image/BuildKernel/MkuImage, \
		none, 0x40008000, 0x40008000, \
		$(BIN_DIR)/$(IMG_PREFIX)-zImage-initramfs, \
		$(BIN_DIR)/$(IMG_PREFIX)-uImage-initramfs \
	)
    endif

	$(foreach board,$(BOARDS),
		$(CP) $(LINUX_DIR)/arch/arm/boot/dts/$(board).dtb $(BIN_DIR)/
	)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-root.$(1) bs=128k conv=sync
endef

$(eval $(call BuildImage))
