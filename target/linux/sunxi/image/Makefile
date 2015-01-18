#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk
include $(INCLUDE_DIR)/host.mk

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_SUNXI_SD_BOOT_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))

BOARDS:= \
	sun4i-a10-cubieboard \
	sun4i-a10-olinuxino-lime \
	sun4i-a10-pcduino \
	sun5i-a13-olinuxino \
	sun6i-a31-colombus \
	sun6i-a31-m9 \
	sun7i-a20-bananapi \
	sun7i-a20-bananapro \
	sun7i-a20-cubieboard2 \
	sun7i-a20-cubietruck \
	sun7i-a20-olinuxino-lime \
	sun7i-a20-olinuxino-micro \
	sun7i-a20-pcduino3

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
		$(CP) $(DTS_DIR)/$(board).dtb $(BIN_DIR)/
	)
endef

define Image/Build/SDCard
	rm -f $(KDIR)/boot.img
	mkdosfs $(KDIR)/boot.img -C $(FAT32_BLOCKS)
	
	mcopy -i $(KDIR)/boot.img $(BIN_DIR)/uboot-sunxi-$(PROFILE)/openwrt-sunxi-$(PROFILE)-boot.scr ::boot.scr
	mcopy -i $(KDIR)/boot.img $(BIN_DIR)/$(2).dtb ::dtb
	mcopy -i $(KDIR)/boot.img $(BIN_DIR)/$(IMG_PREFIX)-uImage ::uImage
	
	./gen_sunxi_sdcard_img.sh \
		$(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-sdcard-vfat-$(1).img \
		$(KDIR)/boot.img \
		$(KDIR)/root.$(1) \
		$(CONFIG_SUNXI_SD_BOOT_PARTSIZE) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) \
		$(BIN_DIR)/uboot-sunxi-$(PROFILE)/openwrt-sunxi-$(PROFILE)-u-boot-with-spl.bin
endef

define Image/Build/Profile/A10-OLinuXino-Lime
	$(call Image/Build/SDCard,$(1),sun4i-a10-olinuxino-lime)
endef

define Image/Build/Profile/A13-OLinuXino
	$(call Image/Build/SDCard,$(1),sun5i-a13-olinuxino)
endef

define Image/Build/Profile/A20-OLinuXino_Lime
	$(call Image/Build/SDCard,$(1),sun7i-a20-olinuxino-lime)
endef

define Image/Build/Profile/A20-OLinuXino_MICRO
	$(call Image/Build/SDCard,$(1),sun7i-a20-olinuxino-micro)
endef

define Image/Build/Profile/Bananapi
	$(call Image/Build/SDCard,$(1),sun7i-a20-bananapi)
endef

define Image/Build/Profile/Bananapro
	$(call Image/Build/SDCard,$(1),sun7i-a20-bananapro)
endef

define Image/Build/Profile/Cubieboard
	$(call Image/Build/SDCard,$(1),sun4i-a10-cubieboard)
endef

define Image/Build/Profile/Cubieboard2
	$(call Image/Build/SDCard,$(1),sun7i-a20-cubieboard2)
endef

define Image/Build/Profile/Cubietruck
	$(call Image/Build/SDCard,$(1),sun7i-a20-cubietruck)
endef

define Image/Build/Profile/OLIMEX_A13_SOM
	$(call Image/Build/SDCard,$(1),sun5i-a13-olinuxino)
endef

define Image/Build/Profile/Mele_M9
	$(call Image/Build/SDCard,$(1),sun6i-a31-hummingbird)
endef

define Image/Build/Profile/Linksprite_pcDuino
	$(call Image/Build/SDCard,$(1),sun4i-a10-pcduino)
endef

define Image/Build/Profile/Linksprite_pcDuino3
	$(call Image/Build/SDCard,$(1),sun7i-a20-pcduino3)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	$(call Image/Build/Profile/$(PROFILE),$(1))
	
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-root.$(1) bs=128k conv=sync
endef

$(eval $(call BuildImage))
