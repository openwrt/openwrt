#
# Copyright (C) 2012-2020 OpenWrt.org
# Copyright (C) 2017 LEDE project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=bcm27xx-gpu-fw
PKG_VERSION:=2020-03-10
PKG_RELEASE:=9e2193537c48abf8225e02e15e319a209cca558a

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)/rpi-firmware-$(PKG_RELEASE)

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

RPI_FIRMWARE_URL:=@GITHUB/raspberrypi/firmware/$(PKG_RELEASE)/boot/
RPI_FIRMWARE_FILE:=rpi-firmware-$(PKG_RELEASE)

define Download/LICENCE_broadcom
  FILE:=$(RPI_FIRMWARE_FILE)-LICENCE.broadcom
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=LICENCE.broadcom
  HASH:=c7283ff51f863d93a275c66e3b4cb08021a5dd4d8c1e7acc47d872fbe52d3d6b
endef
$(eval $(call Download,LICENCE_broadcom))

define Download/bootcode_bin
  FILE:=$(RPI_FIRMWARE_FILE)-bootcode.bin
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=bootcode.bin
  HASH:=1a172fbc75af13411e4928682c72b095287a55007fb6d7233b9bbf9c49afe354
endef
$(eval $(call Download,bootcode_bin))

define Download/fixup_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup.dat
  HASH:=3157bb56f162ed215fba9ed101c5c59abfce25d2723b687d4b6da5d527d4da74
endef
$(eval $(call Download,fixup_dat))

define Download/fixup_cd_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup_cd.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup_cd.dat
  HASH:=1ba8140ee21c61e3c86a0b864530e58de067139b4c93ddd258c12f2f0851796a
endef
$(eval $(call Download,fixup_cd_dat))

define Download/fixup_x_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup_x.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup_x.dat
  HASH:=f0bcb0b9bebceaae272a86f8af5b789a2f1f51548ee428b04bd8f63f8d05dd56
endef
$(eval $(call Download,fixup_x_dat))

define Download/fixup4_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup4.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup4.dat
  HASH:=f6a108c7ffa15b17b6749a4750646cae58fdda4a7b5f182e9fbe77c8f49cdddc
endef
$(eval $(call Download,fixup4_dat))

define Download/fixup4cd_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup4cd.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup4cd.dat
  HASH:=aeac2e302e3e8136df0b312c54f9e290e9f5b8214eba424d24282053c65aa855
endef
$(eval $(call Download,fixup4cd_dat))

define Download/fixup4x_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup4x.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup4x.dat
  HASH:=ce9417b86cc4058a75e80d7b4d0236e78404e279f87c3b700411f8de1cf5da83
endef
$(eval $(call Download,fixup4x_dat))

define Download/start_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start.elf
  HASH:=1e47666306e3570c1c30a6e68dbdf206e696dc0b5105bdde6c0c86340d9e606a
endef
$(eval $(call Download,start_elf))

define Download/start_cd_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start_cd.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start_cd.elf
  HASH:=ac8275d645f40b77210ccafa122b5c9ee17ac6248f6ba17d921f9f669ae9d500
endef
$(eval $(call Download,start_cd_elf))

define Download/start_x_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start_x.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start_x.elf
  HASH:=afb919bdaed4c56e4708c482062fd5158ca08a5dbc66fb95e937838383744884
endef
$(eval $(call Download,start_x_elf))

define Download/start4_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start4.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start4.elf
  HASH:=e38d3dc0991fe6ffcf0ab1f0e2a4c1016b482b4e081604f937d485c548668be4
endef
$(eval $(call Download,start4_elf))

define Download/start4cd_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start4cd.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start4cd.elf
  HASH:=2ec9775e0bffa772119e90c3e1fef017db8d4f22330f6cdf811ca1c56c81da13
endef
$(eval $(call Download,start4cd_elf))

define Download/start4x_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start4x.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start4x.elf
  HASH:=a023230ff6fca93f465e7c36194dda2169b79eb49eacf470860542559952f0b0
endef
$(eval $(call Download,start4x_elf))

define Package/bcm27xx-gpu-fw
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_bcm27xx
  TITLE:=bcm27xx-gpu-fw
  DEFAULT:=y if TARGET_bcm27xx
endef

define Package/bcm27xx-gpu-fw/description
 GPU and kernel boot firmware for bcm27xx.
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) $(DL_DIR)/$(RPI_FIRMWARE_FILE)-LICENCE.broadcom $(PKG_BUILD_DIR)/LICENCE.broadcom
	$(CP) $(DL_DIR)/$(RPI_FIRMWARE_FILE)-bootcode.bin $(PKG_BUILD_DIR)/bootcode.bin
	$(CP) $(DL_DIR)/$(RPI_FIRMWARE_FILE)-fixup.dat $(PKG_BUILD_DIR)/fixup.dat
	$(CP) $(DL_DIR)/$(RPI_FIRMWARE_FILE)-fixup_cd.dat $(PKG_BUILD_DIR)/fixup_cd.dat
	$(CP) $(DL_DIR)/$(RPI_FIRMWARE_FILE)-fixup_x.dat $(PKG_BUILD_DIR)/fixup_x.dat
	$(CP) $(DL_DIR)/$(RPI_FIRMWARE_FILE)-fixup4.dat $(PKG_BUILD_DIR)/fixup4.dat
	$(CP) $(DL_DIR)/$(RPI_FIRMWARE_FILE)-fixup4cd.dat $(PKG_BUILD_DIR)/fixup4cd.dat
	$(CP) $(DL_DIR)/$(RPI_FIRMWARE_FILE)-fixup4x.dat $(PKG_BUILD_DIR)/fixup4x.dat
	$(CP) $(DL_DIR)/$(RPI_FIRMWARE_FILE)-start.elf $(PKG_BUILD_DIR)/start.elf
	$(CP) $(DL_DIR)/$(RPI_FIRMWARE_FILE)-start_cd.elf $(PKG_BUILD_DIR)/start_cd.elf
	$(CP) $(DL_DIR)/$(RPI_FIRMWARE_FILE)-start_x.elf $(PKG_BUILD_DIR)/start_x.elf
	$(CP) $(DL_DIR)/$(RPI_FIRMWARE_FILE)-start4.elf $(PKG_BUILD_DIR)/start4.elf
	$(CP) $(DL_DIR)/$(RPI_FIRMWARE_FILE)-start4cd.elf $(PKG_BUILD_DIR)/start4cd.elf
	$(CP) $(DL_DIR)/$(RPI_FIRMWARE_FILE)-start4x.elf $(PKG_BUILD_DIR)/start4x.elf
endef

define Build/Compile
	true
endef

define Package/bcm27xx-gpu-fw/install
	true
endef

define Build/InstallDev
	$(CP) $(PKG_BUILD_DIR)/bootcode.bin $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/LICENCE.broadcom $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/start.elf $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/start_cd.elf $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/start_x.elf $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/start4.elf $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/start4cd.elf $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/start4x.elf $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/fixup.dat $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/fixup_cd.dat $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/fixup_x.dat $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/fixup4.dat $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/fixup4cd.dat $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/fixup4x.dat $(KERNEL_BUILD_DIR)
endef

$(eval $(call BuildPackage,bcm27xx-gpu-fw))
