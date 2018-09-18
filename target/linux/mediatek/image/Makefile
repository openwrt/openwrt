#
# Copyright (C) 2012-2015 OpenWrt.org
# Copyright (C) 2016-2017 LEDE project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

# for arm
KERNEL_LOADADDR := 0x80008000

# for arm64
ifeq ($(SUBTARGET),mt7622)
KERNEL_LOADADDR = 0x41080000
endif

define Build/sysupgrade-emmc
	rm -f $@.recovery
	mkfs.fat -C $@.recovery 3070

	./gen_mt7623_emmc_img.sh $@ \
		$(IMAGE_KERNEL) \
		$@.recovery \
		$(IMAGE_ROOTFS)
endef



# default all platform image(fit) build 
define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  KERNEL_NAME := zImage
  FILESYSTEMS := squashfs
  DEVICE_DTS_DIR := $(DTS_DIR)
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata 
ifeq ($(SUBTARGET),mt7623)
  KERNEL_NAME := zImage
  KERNEL := kernel-bin | append-dtb | uImage none
  KERNEL_INITRAMFS := kernel-bin | append-dtb | uImage none
endif
ifeq ($(SUBTARGET),mt7622)
  KERNEL_NAME := Image
  KERNEL = kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS = kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
endif
endef

ifeq ($(SUBTARGET),mt7622)
include mt7622.mk
endif

ifeq ($(SUBTARGET),mt7623)
include mt7623.mk
endif

COMPAT_BPI-R2:=bananapi,bpi-r2
COMPAT_EMMC:=mediatek,mt7623-rfb-emmc
COMPAT_NAND:=mediatek,mt7623-rfb-nand
COMPAT_NAND_EPHY:=mediatek,mt7623-rfb-nand-ephy


define Image/Build/BPIR2SDImage
	./make_bundle_image.sh $(KDIR)/mtk-bpi-r2-SD.img \
			       $(STAGING_DIR_IMAGE)/mtk-bpi-r2-preloader-sd.bin \
			       $(STAGING_DIR_IMAGE)/mtk-bpi-r2-uboot.bin \
			       $(KDIR)/uImage-mt7623n-bananapi-bpi-r2 \
			       $(KDIR)/root.squashfs
endef

define Image/Build/BPIR2EMMCImage
	./make_bundle_image.sh $(KDIR)/mtk-bpi-r2-EMMC.img \
			       $(STAGING_DIR_IMAGE)/mtk-bpi-r2-preloader-emmc.bin \
			       $(STAGING_DIR_IMAGE)/mtk-bpi-r2-uboot.bin \
			       $(KDIR)/uImage-mt7623n-bananapi-bpi-r2 \
			       $(KDIR)/root.squashfs
endef
define Image/Build
	$(call Image/Build/$(1),$(1))
	$(call Image/Build/SDImage,$(1))
	$(call Image/Build/EMMCImage,$(1))
	$(call Image/Build/BPIR2SDImage,$(1))
	$(call Image/Build/BPIR2EMMCImage,$(1))
endef
define Image/BuilduImage
	$(CP) $(KDIR)/zImage$(2) $(KDIR)/zImage-$(1)$(2)
	cat $(LINUX_DIR)/arch/arm/boot/dts/$1.dtb >> $(KDIR)/zImage-$(1)$(2)
	mkimage -A arm -O linux -T kernel -C none -a 0x80008000 -e 0x80008000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)'  -d $(KDIR)/zImage-$(1)$(2) $(KDIR)/uImage-$(1)$(2)
endef

define Image/Build/SysupgradeCombined
	$(call Image/BuilduImage,$1)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/BuilduImage,$1,-initramfs)
	$(CP) $(KDIR)/uImage-$(1)-initramfs $(BIN_DIR)/$(IMG_PREFIX)-uImage-$(1)-initramfs
endif
	mkdir -p "$(KDIR_TMP)/sysupgrade-$(3)/"
	echo "BOARD=$(3)" > "$(KDIR_TMP)/sysupgrade-$(3)/CONTROL"
	$(CP) "$(KDIR)/root.$(2)" "$(KDIR_TMP)/sysupgrade-$(3)/root"
	$(CP) "$(KDIR)/uImage-$(1)" "$(KDIR_TMP)/sysupgrade-$(3)/kernel"
	(cd "$(KDIR_TMP)"; $(TAR) cvf \
		"$(BIN_DIR)/$(IMG_PREFIX)-$(3)-sysupgrade.tar" sysupgrade-$(3) \
			$(if $(SOURCE_DATE_EPOCH),--mtime="@$(SOURCE_DATE_EPOCH)") \
	)
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	$(CP) $(KDIR)/root.squashfs $(BIN_DIR)/$(IMG_PREFIX)-root.squashfs

	$(call Image/Build/SysupgradeCombined,mt7623n-bananapi-bpi-r2,squashfs,$$(COMPAT_BPI-R2))
	$(call Image/Build/SysupgradeCombined,mt7623-eMMC,squashfs,$$(COMPAT_EMMC))

	$(call Image/BuilduImage,mt7623-NAND)
	$(call Image/BuilduImage,mt7623-NAND-ePHY)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/BuilduImage,mt7623-NAND,-initramfs)
	$(call Image/BuilduImage,mt7623-NAND-ePHY,-initramfs)
	$(CP) $(KDIR)/uImage-mt7623-NAND-initramfs $(BIN_DIR)/$(IMG_PREFIX)-uImage-NAND-initramfs
	$(CP) $(KDIR)/uImage-mt7623-NAND-ePHY-initramfs $(BIN_DIR)/$(IMG_PREFIX)-uImage-NAND-ePHY-initramfs
endif
	$(call Image/Build/SysupgradeNAND,mt7623-NAND,$(1),$(KDIR)/uImage-mt7623-NAND,$$(COMPAT_NAND))
	$(call Image/Build/SysupgradeNAND,mt7623-NAND-ePHY,$(1),$(KDIR)/uImage-mt7623-NAND-ePHY,$$(COMPAT_NAND_EPHY))
endef




$(eval $(call BuildImage))

