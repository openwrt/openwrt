#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk
include $(INCLUDE_DIR)/host.mk

DEVICE_VARS += DEVICE_DTS DEVICE_PROFILE IMAGE_SIZE DTB_SIZE

define Device/Default
  KERNEL_DEPENDS = $$(wildcard ../dts/$$(DEVICE_DTS).dts)
  DEVICE_PROFILE :=
  DEVICE_DTS :=
  KERNEL_ENTRY := 0x00000000
  KERNEL_LOADADDR := 0x00000000
endef

define Build/dtb
	$(call Image/BuildDTB,../dts/$(DEVICE_DTS).dts,$@.dtb,,--space $(DTB_SIZE))
endef

ifeq ($(SUBTARGET),nand)

define Image/cpiogz
	( cd $(TARGET_DIR); find . | cpio -o -H newc | gzip -9n >$(KDIR_TMP)/$(IMG_PREFIX)-rootfs.cpio.gz )
endef

define Build/copy-file
	cat "$(1)" > "$@"
endef

define Build/MerakiAdd-dtb
	$(call Image/BuildDTB,../dts/$(DEVICE_DTS).dts,$@.dtb)
	( \
		dd if=$@.dtb bs=$(DTB_SIZE) conv=sync; \
		dd if=$@ bs=$(BLOCKSIZE) conv=sync; \
	) > $@.new
	@mv $@.new $@
endef

define Build/MerakiAdd-initramfs
	$(call Image/cpiogz)

	-$(STAGING_DIR_HOST)/bin/mkimage -A $(LINUX_KARCH) -O linux -T ramdisk \
		-C gzip -n "$(PROFILE) rootfs" \
		-d $(KDIR_TMP)/$(IMG_PREFIX)-rootfs.cpio.gz \
		$(KDIR_TMP)/$(IMG_PREFIX)-uramdisk.image.gz

	( \
		dd if=$@ bs=1k conv=sync; \
		dd if=$(KDIR_TMP)/$(IMG_PREFIX)-uramdisk.image.gz bs=$(BLOCKSIZE) conv=sync; \
	) > $@.new
	@mv $@.new $@
endef

define Build/MerakiNAND
	-$(STAGING_DIR_HOST)/bin/mkmerakifw \
		-B $(DEVICE_PROFILE) -s \
		-i $@ \
		-o $@.new
	@cp $@.new $@
endef

define Device/mr24
  DEVICE_TITLE := Cisco Meraki MR24
  DEVICE_PACKAGES := kmod-spi-gpio kmod-ath9k wpad-mini
  DEVICE_PROFILE := MR24
  DEVICE_DTS := MR24
  BLOCKSIZE := 64512
  IMAGES := sysupgrade.tar
  DTB_SIZE := 64512
  KERNEL_SIZE := 2048k
  IMAGE_SIZE := 8191k
  KERNEL := kernel-bin | lzma | uImage lzma | MerakiAdd-dtb | MerakiNAND
  KERNEL_INITRAMFS := copy-file $(KDIR)/vmlinux | lzma | uImage lzma | MerakiAdd-dtb | pad-to 2047k | MerakiAdd-initramfs | MerakiNAND
  IMAGE/sysupgrade.tar := sysupgrade-nand
endef
TARGET_DEVICES += mr24

endif

ifeq ($(SUBTARGET),sata)

### Image scripts for the WD My Book Live Series ###
define Build/boot-script
	$(STAGING_DIR_HOST)/bin/mkimage -A powerpc -T script -C none -n "$(PROFILE) Boot Script" \
		-d mbl_boot.scr \
		$@.scr
endef

define Build/boot-img
	$(RM) -rf $@.bootdir
	mkdir -p $@.bootdir/boot

	$(CP) $@.scr $@.bootdir/boot/boot.scr
	$(CP) $@.dtb $@.bootdir/boot/$(DEVICE_DTB)
	$(CP) $(word 1,$^) $@.bootdir/boot/uImage

	genext2fs --block-size $(BLOCKSIZE) --size-in-blocks $$((1024 * $(BOOT_SIZE))) --root $@.bootdir $@.boot

	# convert it to revision 1 - needed for u-boot ext2load
	$(STAGING_DIR_HOST)/bin/tune2fs -O filetype $@.boot
	$(STAGING_DIR_HOST)/bin/e2fsck -pDf $@.boot > /dev/null
endef

define Build/hdd-img
	./mbl_gen_hdd_img.sh $@ $@.boot $(word 2,$^)
	$(if $(CONFIG_TARGET_IMAGES_GZIP),gzip -9n -c $@ > $(BIN_DIR)/$(notdir $@).gz)
endef

define Build/uRamdisk
	$(call Image/mkfs/ext4)
	gzip -9n -c $(KDIR)/root.ext4 > $(KDIR)/root.ext4.gz
	$(STAGING_DIR_HOST)/bin/mkimage -A powerpc -T ramdisk -C gzip -n "$(PROFILE) rootfs" \
		-d $(KDIR)/root.ext4.gz \
		$@.uRamdisk
endef

define Build/recovery-tar
	sh ./mbl_gen_recovery_tar.sh \
		--profile $(DEVICE_PROFILE) \
		--dtb $@.dtb \
		--dtbname $(DEVICE_DTB) \
		--kernel $@	 \
		--rootfs $@.uRamdisk \
		$@
endef

define Device/MyBookLiveDefault
  IMAGE_SIZE := 48m
  BLOCKSIZE := 1024
  DTB_SIZE := 16384
  KERNEL := kernel-bin | gzip | uImage gzip
  BOOT_SIZE := 8
  IMAGES := rootfs.img recovery.tar
  DEVICE_DTB := apollo3g.dtb
  IMAGE/rootfs.img := boot-script | dtb | boot-img | hdd-img
  IMAGE/recovery.tar := kernel-bin | dtb | uRamdisk | recovery-tar
endef

define Device/MyBookLiveSingle
$(Device/MyBookLiveDefault)
  DEVICE_TITLE := Western Digital My Book Live
  DEVICE_DTS := apollo3g
  DEVICE_PROFILE := apollo3g
endef

TARGET_DEVICES += MyBookLiveSingle

define Device/MyBookLiveDuo
$(Device/MyBookLiveDefault)
  DEVICE_TITLE := Western Digital My Book Live Duo
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-ledtrig-usbdev kmod-usb-storage kmod-fs-vfat wpad-mini
  DEVICE_DTS := apollo3g-duo
  DEVICE_PROFILE := ap2nc
endef

TARGET_DEVICES += MyBookLiveDuo

endif

$(eval $(call BuildImage))

