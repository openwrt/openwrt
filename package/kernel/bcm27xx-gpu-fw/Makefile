include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=bcm27xx-gpu-fw
PKG_VERSION:=2022-02-09
PKG_RELEASE:=577aef2bc2f8e9dcd6955a85cf0621cc73b29db7

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)/rpi-firmware-$(PKG_RELEASE)

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

RPI_FIRMWARE_URL:=@GITHUB/raspberrypi/firmware/$(PKG_RELEASE)/boot/
RPI_FIRMWARE_FILE:=rpi-firmware-$(PKG_RELEASE)

define Download/LICENCE_broadcom
  FILE:=$(RPI_FIRMWARE_FILE)-LICENCE.broadcom
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=LICENCE.broadcom
  HASH:=d0f5a01a474aee406f849ba5153ba6a890b4b285aabc9b96a6e35504ae664a88
endef
$(eval $(call Download,LICENCE_broadcom))

define Download/bootcode_bin
  FILE:=$(RPI_FIRMWARE_FILE)-bootcode.bin
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=bootcode.bin
  HASH:=69309823da13dc96b89e3d82b44f820e4f84efa79d207adad2c8784559794f03
endef
$(eval $(call Download,bootcode_bin))

define Download/fixup_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup.dat
  HASH:=dcbb72ef0e7ed3b991036a1e4b4b61957b16ce6ed5784504cc7ecabe1c780da0
endef
$(eval $(call Download,fixup_dat))

define Download/fixup_cd_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup_cd.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup_cd.dat
  HASH:=35a9e40cb8a7d5a2a962532b5c8e852ff285cebeaecf1b58ca3d3afacce8cb35
endef
$(eval $(call Download,fixup_cd_dat))

define Download/fixup_x_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup_x.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup_x.dat
  HASH:=34d1347daf92d8aa75197f17ce4caadd700d10bf116661e37a9436b958244901
endef
$(eval $(call Download,fixup_x_dat))

define Download/fixup4_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup4.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup4.dat
  HASH:=9d108661b8f758cf7f6cae1666926a85af858eac4c8cef06dfebef1192474129
endef
$(eval $(call Download,fixup4_dat))

define Download/fixup4cd_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup4cd.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup4cd.dat
  HASH:=35a9e40cb8a7d5a2a962532b5c8e852ff285cebeaecf1b58ca3d3afacce8cb35
endef
$(eval $(call Download,fixup4cd_dat))

define Download/fixup4x_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup4x.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup4x.dat
  HASH:=3315664a979968ad7761b35a1a37224539e2ba57a32441d56fbfbce2fd2683ff
endef
$(eval $(call Download,fixup4x_dat))

define Download/start_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start.elf
  HASH:=25ae5bd9fd19fc45f83bf632011e1a3e690f4903b58f73affda236f6210b8700
endef
$(eval $(call Download,start_elf))

define Download/start_cd_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start_cd.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start_cd.elf
  HASH:=12d64f6748bc4655ccbbdcb1ed86d36271d4ed972947d7a0170ab2972fefa44b
endef
$(eval $(call Download,start_cd_elf))

define Download/start_x_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start_x.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start_x.elf
  HASH:=e7690ff5a77a0b97d433c22f0310b3caebeacf171b074b3fc4682c469ef69582
endef
$(eval $(call Download,start_x_elf))

define Download/start4_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start4.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start4.elf
  HASH:=a898ff5f188bf72453606f31802df5727d1db93cf55d8106af6ffed5f0de4c3f
endef
$(eval $(call Download,start4_elf))

define Download/start4cd_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start4cd.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start4cd.elf
  HASH:=70f2a540cfd7050d394730f5cf9fe4aff91954639d9dac5ca385354235ddbd98
endef
$(eval $(call Download,start4cd_elf))

define Download/start4x_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start4x.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start4x.elf
  HASH:=4c7b62e07b5bc5eb50d8804a69f57695f773340e9a6c9450d9e723bf25e3c477
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
