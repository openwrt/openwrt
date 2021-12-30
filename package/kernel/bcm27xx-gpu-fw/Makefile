include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=bcm27xx-gpu-fw
PKG_VERSION:=2021-11-17
PKG_RELEASE:=12bc6e3677348adaffd155e7a04761e2661d4bff

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
  HASH:=69309823da13dc96b89e3d82b44f820e4f84efa79d207adad2c8784559794f03
endef
$(eval $(call Download,bootcode_bin))

define Download/fixup_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup.dat
  HASH:=216de5941d36b43f0303b07594610770e2cac5f3bf93aed8b704bf588e473e1e
endef
$(eval $(call Download,fixup_dat))

define Download/fixup_cd_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup_cd.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup_cd.dat
  HASH:=19ede6d6923db848bdaff739fe17a0fc3f8de63062d51be1db67af850fcf6af1
endef
$(eval $(call Download,fixup_cd_dat))

define Download/fixup_x_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup_x.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup_x.dat
  HASH:=454f6f0242ee3b670d1b3da71a83de631aec522ca689b6579a78127b1b4940e1
endef
$(eval $(call Download,fixup_x_dat))

define Download/fixup4_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup4.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup4.dat
  HASH:=a1b7dac827eea10f66b0fd41292a8b1628ce04a228e1e5068aba1cbd5c759a70
endef
$(eval $(call Download,fixup4_dat))

define Download/fixup4cd_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup4cd.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup4cd.dat
  HASH:=19ede6d6923db848bdaff739fe17a0fc3f8de63062d51be1db67af850fcf6af1
endef
$(eval $(call Download,fixup4cd_dat))

define Download/fixup4x_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup4x.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup4x.dat
  HASH:=8fe9635bfa6dfc5a06b4cc1cb4c0fbd4f7aacb4b18591b3185973a7ea17cb6c7
endef
$(eval $(call Download,fixup4x_dat))

define Download/start_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start.elf
  HASH:=b663fdc19e9acf5e406ccbb23a30dce8f054a4b1a14d598ed2c977585e192848
endef
$(eval $(call Download,start_elf))

define Download/start_cd_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start_cd.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start_cd.elf
  HASH:=49c9c0cf6d4c4bb4c395499b6bf9eb5f7953f3a44d6375d365ff441301694f73
endef
$(eval $(call Download,start_cd_elf))

define Download/start_x_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start_x.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start_x.elf
  HASH:=3b93ad731821a2eeec20ee21ca87b06f1edba66325a9fa41804f687fadd1bcda
endef
$(eval $(call Download,start_x_elf))

define Download/start4_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start4.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start4.elf
  HASH:=3a9f855c2d54974bf141b3bde17a1ad35b424687ed155ba5184fd62366b77541
endef
$(eval $(call Download,start4_elf))

define Download/start4cd_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start4cd.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start4cd.elf
  HASH:=11eb1b72ce2d00eb9150d076214b374bfaf64842bdc10464be768a161ce3c3cf
endef
$(eval $(call Download,start4cd_elf))

define Download/start4x_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start4x.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start4x.elf
  HASH:=c9343efbe8b81d72550a16c12fefdb9cc60a178d217a06fbde022d8f97a5a944
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
