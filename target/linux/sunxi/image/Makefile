#
# Copyright (C) 2013-2016 OpenWrt.org
# Copyright (C) 2016 Yousong Zhou
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk
include $(INCLUDE_DIR)/host.mk

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_SUNXI_SD_BOOT_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))

KERNEL_LOADADDR:=0x40008000

define Build/sunxi-sdcard
	rm -f $@.boot
	mkfs.fat $@.boot -C $(FAT32_BLOCKS)

	mcopy -i $@.boot $(KDIR)/uboot-sunxi-$(SUNXI_UBOOT)-boot.scr ::boot.scr
	mcopy -i $@.boot $(DTS_DIR)/$(SUNXI_DTS).dtb ::dtb
	mcopy -i $@.boot $(IMAGE_KERNEL) ::uImage
	./gen_sunxi_sdcard_img.sh $@ \
		$@.boot \
		$(IMAGE_ROOTFS) \
		$(CONFIG_SUNXI_SD_BOOT_PARTSIZE) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) \
		$(KDIR)/uboot-sunxi-$(SUNXI_UBOOT)-u-boot-with-spl.bin
	rm -f $@.boot
endef

# why \x00\x00\x00\x00 for zImage-initramfs
define Device/Default
  PROFILES := Default
  DEVICE_VARS := SUNXI_DTS SUNXI_UBOOT
  KERNEL_NAME := zImage
  KERNEL := kernel-bin | uImage none
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := sunxi-sdcard | append-metadata | gzip
endef

define Device/sun4i-a10-olinuxino-lime
  DEVICE_TITLE:=Olimex A10-OLinuXino-LIME
  DEVICE_PACKAGES:=uboot-sunxi-A10-OLinuXino-Lime kmod-ata-core kmod-ata-sunxi kmod-sun4i-emac kmod-rtc-sunxi
  SUPPORTED_DEVICES:=olimex,a10-olinuxino-lime
  SUNXI_DTS:=sun4i-a10-olinuxino-lime
  SUNXI_UBOOT:=A10-OLinuXino-Lime
endef

TARGET_DEVICES += sun4i-a10-olinuxino-lime


define Device/sun5i-a13-olimex-som
  DEVICE_TITLE:=Olimex A13 SOM
  DEVICE_PACKAGES:=uboot-sunxi-OLIMEX_A13_SOM kmod-rtl8192cu
  SUPPORTED_DEVICES:=olimex,a13-olinuxino
  SUNXI_DTS:=sun5i-a13-olinuxino
  SUNXI_UBOOT:=OLIMEX_A13_SOM
endef

TARGET_DEVICES += sun5i-a13-olimex-som


define Device/sun5i-a13-olinuxino
  DEVICE_TITLE:=Olimex A13-Olinuxino
  DEVICE_PACKAGES:=uboot-sunxi-A13-OLinuXino kmod-rtl8192cu
  SUPPORTED_DEVICES:=olimex,a13-olinuxino
  SUNXI_DTS:=sun5i-a13-olinuxino
  SUNXI_UBOOT:=A13-OLinuXino
endef

TARGET_DEVICES += sun5i-a13-olinuxino


define Device/sun7i-a20-olinuxino-lime
  DEVICE_TITLE:=Olimex A20-OLinuXino-LIME
  DEVICE_PACKAGES:=uboot-sunxi-A20-OLinuXino-Lime kmod-ata-core kmod-ata-sunxi kmod-rtc-sunxi
  SUPPORTED_DEVICES:=olimex,a20-olinuxino-lime
  SUNXI_DTS:=sun7i-a20-olinuxino-lime
  SUNXI_UBOOT:=A20-OLinuXino-Lime
endef

TARGET_DEVICES += sun7i-a20-olinuxino-lime


define Device/sun7i-a20-olinuxino-micro
  DEVICE_TITLE:=Olimex A20-Olinuxino Micro
  DEVICE_PACKAGES:=uboot-sunxi-A20-OLinuXino_MICRO kmod-ata-core kmod-ata-sunxi kmod-sun4i-emac kmod-rtc-sunxi
  SUPPORTED_DEVICES:=olimex,a20-olinuxino-micro
  SUNXI_DTS:=sun7i-a20-olinuxino-micro
  SUNXI_UBOOT:=A20-OLinuXino_MICRO
endef

TARGET_DEVICES += sun7i-a20-olinuxino-micro


define Device/sun7i-a20-bananapi
  DEVICE_TITLE:=LeMaker Banana Pi
  DEVICE_PACKAGES:=uboot-sunxi-Bananapi kmod-rtc-sunxi kmod-ata-core kmod-ata-sunxi
  SUPPORTED_DEVICES:=lemaker,bananapi
  SUNXI_DTS:=sun7i-a20-bananapi
  SUNXI_UBOOT:=Bananapi
endef

TARGET_DEVICES += sun7i-a20-bananapi


define Device/sun7i-a20-bananapro
  DEVICE_TITLE:=LeMaker Banana Pro
  DEVICE_PACKAGES:=uboot-sunxi-Bananapro kmod-rtc-sunxi kmod-ata-core kmod-ata-sunxi kmod-brcmfmac
  SUPPORTED_DEVICES:=lemaker,bananapro
  SUNXI_DTS:=sun7i-a20-bananapro
  SUNXI_UBOOT:=Bananapro
endef

TARGET_DEVICES += sun7i-a20-bananapro


define Device/sun7i-a20-cubieboard2
  DEVICE_TITLE:=Cubietech Cubieboard2
  DEVICE_PACKAGES:=uboot-sunxi-Cubieboard2 kmod-ata-core kmod-ata-sunxi kmod-sun4i-emac kmod-rtc-sunxi
  SUPPORTED_DEVICES:=cubietech,cubieboard2
  SUNXI_DTS:=sun7i-a20-cubieboard2
  SUNXI_UBOOT:=Cubieboard2
endef

TARGET_DEVICES += sun7i-a20-cubieboard2


define Device/sun4i-a10-cubieboard
  DEVICE_TITLE:=Cubietech Cubieboard
  DEVICE_PACKAGES:=uboot-sunxi-Cubieboard kmod-ata-core kmod-ata-sunxi kmod-sun4i-emac kmod-rtc-sunxi
  SUPPORTED_DEVICES:=cubietech,a10-cubieboard
  SUNXI_DTS:=sun4i-a10-cubieboard
  SUNXI_UBOOT:=Cubieboard
endef

TARGET_DEVICES += sun4i-a10-cubieboard


define Device/sun7i-a20-cubietruck
  DEVICE_TITLE:=Cubietech Cubietruck
  DEVICE_PACKAGES:=uboot-sunxi-Cubietruck kmod-ata-core kmod-ata-sunxi kmod-rtc-sunxi kmod-brcmfmac
  SUPPORTED_DEVICES:=cubietech,cubietruck
  SUNXI_DTS:=sun7i-a20-cubietruck
  SUNXI_UBOOT:=Cubietruck
endef

TARGET_DEVICES += sun7i-a20-cubietruck


define Device/sun7i-a20-lamobo-r1
  DEVICE_TITLE:=Lamobo R1
  DEVICE_PACKAGES:=uboot-sunxi-Lamobo_R1 kmod-ata-sunxi kmod-rtl8192cu swconfig wpad-mini
  SUPPORTED_DEVICES:=lamobo,lamobo-r1
  SUNXI_DTS:=sun7i-a20-lamobo-r1
  SUNXI_UBOOT:=Lamobo_R1
endef

TARGET_DEVICES += sun7i-a20-lamobo-r1


define Device/sun6i-a31-m9
  DEVICE_TITLE:=Mele M9 top set box
  DEVICE_PACKAGES:=uboot-sunxi-Mele_M9 kmod-sun4i-emac kmod-rtc-sunxi kmod-rtl8192cu
  SUPPORTED_DEVICES:=mele,m9
  SUNXI_DTS:=sun6i-a31-m9
  SUNXI_UBOOT:=Mele_M9
endef

TARGET_DEVICES += sun6i-a31-m9


define Device/sun8i-h3-orangepi-plus
  DEVICE_TITLE:=Xunlong Orange Pi Plus
  DEVICE_PACKAGES:=kmod-rtc-sunxi uboot-sunxi-orangepi_plus
  SUPPORTED_DEVICES:=xunlong,orangepi-plus
  SUNXI_DTS:=sun8i-h3-orangepi-plus
  SUNXI_UBOOT:=orangepi_plus
endef

TARGET_DEVICES += sun8i-h3-orangepi-plus


define Device/sun7i-a20-pcduino3
  DEVICE_TITLE:=LinkSprite pcDuino3
  DEVICE_PACKAGES:=uboot-sunxi-Linksprite_pcDuino3 kmod-sun4i-emac kmod-rtc-sunxi kmod-ata-core kmod-ata-sunxi kmod-rtl8xxxu rtl8188eu-firmware
  SUPPORTED_DEVICES:=linksprite,pcduino3
  SUNXI_DTS:=sun7i-a20-pcduino3
  SUNXI_UBOOT:=Linksprite_pcDuino3
endef

TARGET_DEVICES += sun7i-a20-pcduino3


define Device/sun4i-a10-pcduino
  DEVICE_TITLE:=LinkSprite pcDuino
  DEVICE_PACKAGES:=uboot-sunxi-Linksprite_pcDuino kmod-sun4i-emac kmod-rtc-sunxi kmod-rtl8192cu
  SUPPORTED_DEVICES:=linksprite,a10-pcduino
  SUNXI_DTS:=sun4i-a10-pcduino
  SUNXI_UBOOT:=Linksprite_pcDuino
endef

TARGET_DEVICES += sun4i-a10-pcduino


$(eval $(call BuildImage))
