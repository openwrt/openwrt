#
# Copyright (C) 2013-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk
include $(INCLUDE_DIR)/host.mk

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_SUNXI_SD_BOOT_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))

define Image/BuildKernel
	-mkdir -p $(KDIR_TMP)

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
endef

define Image/Build/SDCard
	rm -f $(KDIR_TMP)/$(IMG_PREFIX)-$(PROFILE)-boot.img
	mkdosfs $(KDIR_TMP)/$(IMG_PREFIX)-$(PROFILE)-boot.img -C $(FAT32_BLOCKS)

	mcopy -i $(KDIR_TMP)/$(IMG_PREFIX)-$(PROFILE)-boot.img $(KDIR)/uboot-sunxi-$(PROFILE)-boot.scr ::boot.scr
	mcopy -i $(KDIR_TMP)/$(IMG_PREFIX)-$(PROFILE)-boot.img $(DTS_DIR)/$(2).dtb ::dtb
	mcopy -i $(KDIR_TMP)/$(IMG_PREFIX)-$(PROFILE)-boot.img $(BIN_DIR)/$(IMG_PREFIX)-uImage ::uImage

	./gen_sunxi_sdcard_img.sh \
		$(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-sdcard-vfat-$(1).img \
		$(KDIR_TMP)/$(IMG_PREFIX)-$(PROFILE)-boot.img \
		$(KDIR)/root.$(1) \
		$(CONFIG_SUNXI_SD_BOOT_PARTSIZE) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) \
		$(KDIR)/uboot-sunxi-$(PROFILE)-u-boot-with-spl.bin

  ifneq ($(CONFIG_TARGET_IMAGES_GZIP),)
	gzip -f9n $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-sdcard-vfat-$(1).img
  endif
endef

define Image/Build/Profile/A10-OLinuXino-Lime
	$(call Image/Build/SDCard,$(1),sun4i-a10-olinuxino-lime)
endef

define Image/Build/Profile/A13-OLinuXino
	$(call Image/Build/SDCard,$(1),sun5i-a13-olinuxino)
endef

define Image/Build/Profile/A20-OLinuXino-Lime
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

define Image/Build/Profile/Lamobo_R1
	$(call Image/Build/SDCard,$(1),sun7i-a20-lamobo-r1)
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

define Image/Build/Profile/orangepi_plus
	$(call Image/Build/SDCard,$(1),sun8i-h3-orangepi-plus)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	$(call Image/Build/Profile/$(PROFILE),$(1))

	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
endef

$(eval $(call BuildImage))
