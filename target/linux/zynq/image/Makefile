#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

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
		-D $(1) -o $(KDIR)/fit-$(1).its -k $(KDIR)/zImage \
		-d $(BIN_DIR)/$(IMG_PREFIX)-system.dtb \
		-C none -a $(KERNEL_LOADADDR) -e $(KERNEL_ENTRY_POINT) \
		-A $(ARCH) -v $(LINUX_VERSION) \
		-r $(BIN_DIR)/$(IMG_PREFIX)-rootfs.cpio.gz -z gzip
	PATH=$(LINUX_DIR)/scripts/dtc:$(PATH) mkimage -f $(KDIR)/fit-$(1).its $(KDIR)/fit-$(1).itb
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-fit.itb
	ln -fs $(IMG_PREFIX)-fit.itb $(BIN_DIR)/fit.itb

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

### ZC702 ###
define Device/ZC702
	DEVICE_DTS := zynq-zc702
endef

Image/boot-imgs/ZC702=$(call Image/boot-imgs,zc702)

define Image/BuildKernel
	$(eval $(call Device/$(PROFILE)))
	$(call Image/boot-imgs/$(PROFILE))
endef

$(eval $(call BuildImage))
