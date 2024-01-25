
define Device/default-nand
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  SUBPAGESIZE := 2048
  MKUBIFS_OPTS := -m $$(PAGESIZE) -e 124KiB -c 2048
endef

define Build/at91-sdcard
  $(if $(findstring ext4,$@), \
  rm -f $@.boot
  mkfs.fat -C $@.boot $(FAT32_BLOCKS)

  mcopy -i $@.boot \
	$(KDIR)/$(DEVICE_NAME)-fit-zImage.itb \
	::$(DEVICE_NAME)-fit.itb

  mcopy -i $@.boot \
	$(BIN_DIR)/u-boot-$(DEVICE_DTS:at91-%=%)_mmc1/u-boot.bin \
	::u-boot.bin

  mcopy -i $@.boot \
	$(BIN_DIR)/at91bootstrap-$(DEVICE_DTS:at91-%=%)sd_uboot/at91bootstrap.bin \
	::BOOT.bin

  $(CP) uboot-env.txt $@-uboot-env.txt
  sed -i '2d;3d' $@-uboot-env.txt
  sed -i '2i board='"$(DEVICE_NAME)"'' $@-uboot-env.txt
  sed -i '3i board_name='"$(firstword $(SUPPORTED_DEVICES))"'' $@-uboot-env.txt

  mkenvimage -s 0x4000 -o $@-uboot.env $@-uboot-env.txt

  mcopy -i $@.boot $@-uboot.env ::uboot.env

  ./gen_at91_sdcard_img.sh \
	$@.img \
	$@.boot \
	$(IMAGE_ROOTFS) \
	$(AT91_SD_BOOT_PARTSIZE) \
	$(CONFIG_TARGET_ROOTFS_PARTSIZE)

  gzip -nc9 $@.img > $@

  rm -f $@.img $@.boot $@-uboot.env $@-uboot-env.txt)
endef

define Device/microchip_sama7g5-ek
  $(Device/evaluation-dtb)
  DEVICE_VENDOR := Microchip
  DEVICE_MODEL := SAMA7G5-EK
  DEVICE_DTS := at91-sama7g5ek
  SUPPORTED_DEVICES := microchip,sama7g5ek
  KERNEL_SIZE := 6144k
  KERNEL_LOADADDR := 0x62000000
  $(Device/evaluation-sdimage)
endef
TARGET_DEVICES += microchip_sama7g5-ek
