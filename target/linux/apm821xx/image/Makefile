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

$(eval $(call BuildImage))

endif
