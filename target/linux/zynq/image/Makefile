#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk
include $(INCLUDE_DIR)/host.mk

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_ZYNQ_SD_BOOT_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))

#################################################
# Images
#################################################

define Image/boot-imgs
	# Copy zImage to BIN_DIR
	$(CP) $(KDIR)/zImage $(BIN_DIR)/$(IMG_PREFIX)-zImage

	$(call Image/BuildKernel/MkuImage, none, $(KERNEL_LOADADDR), \
		$(KERNEL_ENTRY_POINT), $(KDIR)/zImage, \
		$(BIN_DIR)/$(IMG_PREFIX)-uImage)

	$(call Image/BuildDTB,$(DTS_DIR)/$(DEVICE_DTS).dts,\
		$(BIN_DIR)/$(IMG_PREFIX)-system.dtb)

	$(call Image/mkfs/cpiogz)
	$(call Image/mkfs/targz)

	# Create uboot cpio.gz
	mkimage -A arm -T ramdisk -C gzip -n "$(PROFILE) OpenWRT rootfs" \
		-d $(BIN_DIR)/$(IMG_PREFIX)-rootfs.cpio.gz \
		$(BIN_DIR)/$(IMG_PREFIX)-uramdisk.image.gz

	# create FIT image with rootfs
	./mkits.sh \
		-D $(DEVICE_LC) -o $(KDIR)/fit-$(DEVICE_LC).its -k $(KDIR)/zImage \
		-d $(BIN_DIR)/$(IMG_PREFIX)-system.dtb \
		-C none -a $(KERNEL_LOADADDR) -e $(KERNEL_ENTRY_POINT) \
		-A $(ARCH) -v $(LINUX_VERSION) \
		-r $(BIN_DIR)/$(IMG_PREFIX)-rootfs.cpio.gz -z gzip
	PATH=$(LINUX_DIR)/scripts/dtc:$(PATH) mkimage -f $(KDIR)/fit-$(DEVICE_LC).its $(KDIR)/fit-$(DEVICE_LC).itb
	$(CP) $(KDIR)/fit-$(DEVICE_LC).itb $(BIN_DIR)/$(IMG_PREFIX)-fit.itb
	ln -fs $(IMG_PREFIX)-fit.itb $(BIN_DIR)/fit.itb
endef

define Image/Build/Zynq
	# Create SD card image
	rm -f $(KDIR_TMP)/$(IMG_PREFIX)-boot.img
	mkfs.fat $(KDIR_TMP)/$(IMG_PREFIX)-boot.img -C $(FAT32_BLOCKS)
	mcopy -i $(KDIR_TMP)/$(IMG_PREFIX)-boot.img $(KDIR)/uboot-$(IMG_PREFIX)-u-boot.bin ::boot.bin
	mcopy -i $(KDIR_TMP)/$(IMG_PREFIX)-boot.img $(BIN_DIR)/$(DEVICE_DTS)-system.dtb ::devicetree.dtb
	mcopy -i $(KDIR_TMP)/$(IMG_PREFIX)-boot.img $(KDIR)/uboot-$(IMG_PREFIX)-u-boot-dtb.img ::u-boot.img
	mcopy -i $(KDIR_TMP)/$(IMG_PREFIX)-boot.img $(KDIR)/uboot-$(IMG_PREFIX)-uEnv.txt ::uEnv.txt
	mcopy -i $(KDIR_TMP)/$(IMG_PREFIX)-boot.img $(BIN_DIR)/$(IMG_PREFIX)-uImage ::uImage

	./gen_zynq_sdcard_img.sh \
		$(BIN_DIR)/$(IMG_PREFIX)-sdcard-vfat-$(1).img \
		$(KDIR_TMP)/$(IMG_PREFIX)-boot.img \
		$(KDIR)/root.$(1) \
		$(CONFIG_ZYNQ_SD_BOOT_PARTSIZE) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE)

	$(if $(CONFIG_TARGET_IMAGES_GZIP),gzip -9n -c $(BIN_DIR)/$(IMG_PREFIX)-sdcard-vfat-$(1).img > $(BIN_DIR)/$(IMG_PREFIX)-sdcard-vfat-$(1).img.gz)
endef

#################################################
# Devices
#################################################

# default kernel load address
KERNEL_LOADADDR=0x8000
KERNEL_ENTRY_POINT=0x8000

### Device macros ###
define Device/Default
	IMG_PREFIX := zynq
	DEVICE_DTS :=
	PROFILE_SANITIZED :=
endef
DEVICE_VARS += DEVICE_DTS

define Device/DefaultConfig
	DEVICE_LC = $(1)
	IMG_PREFIX = zynq-$(1)
	DEVICE_DTS = zynq-$(1)
endef

### ZC702 ###
define Device/ZC702
	$(call Device/DefaultConfig,zc702)
endef

define Device/ZED
	$(call Device/DefaultConfig,zed)
endef

define Device/ZYBO
	$(call Device/DefaultConfig,zybo)
endef

define Image/BuildKernel
	$(eval $(call Device/$(PROFILE)))
	$(call Image/boot-imgs)
endef

define Image/Build
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1).img

	$(call Image/Build/Zynq,$(1))
endef

$(eval $(call BuildImage))
