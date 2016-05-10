#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_SOCFPGA_SD_BOOT_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))
KDIR_TMP:=$(KDIR)/tmp
KDIR_TMP_EXT4:=$(KDIR)/tmp-ext4

# Terasic SoCkit: QSPI NOR, N25Q00A
SOCFPGA_SOCKIT_UBIFS_OPTS="-m 1 -e 65408 -c 2040"
SOCFPGA_SOCKIT_UBI_OPTS="-m 1 -p 64KiB -s 1"

define sanitize_profile_name
$(shell echo $(PROFILE) | tr '[:upper:]' '[:lower:]' | sed 's/_/-/g')
endef

define Image/BuildKernel/Template

 ifneq ($(1),)
	$(CP) $(DTS_DIR)/$(1).dtb $(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb

	$(call Image/BuildKernel/MkFIT,$(1),$(KDIR)/zImage,$(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb,none,0x00008000,0x00008000)
	$(CP) $(KDIR)/fit-$(1).itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage.itb

  ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/BuildKernel/MkFIT,$(1),$(KDIR)/zImage-initramfs,$(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb,none,0x00008000,0x00008000,-initramfs)
	$(CP) $(KDIR)/fit-$(1)-initramfs.itb $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fit-uImage-initramfs.itb
  endif
 endif

	$(CP) $(KDIR)/zImage $(BIN_DIR)/$(IMG_PREFIX)-zImage
	$(call Image/BuildKernel/MkuImage, \
		none, 0x00008000, 0x00008000, \
		$(BIN_DIR)/$(IMG_PREFIX)-zImage, \
		$(BIN_DIR)/$(IMG_PREFIX)-uImage \
	)

 ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(CP) $(KDIR)/zImage-initramfs $(BIN_DIR)/$(IMG_PREFIX)-zImage-initramfs
	$(call Image/BuildKernel/MkuImage, \
		none, 0x00008000, 0x00008000, \
		$(BIN_DIR)/$(IMG_PREFIX)-zImage-initramfs, \
		$(BIN_DIR)/$(IMG_PREFIX)-uImage-initramfs \
	)
 endif
endef

define Image/InstallKernel/Template

 ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_KERNEL)$(CONFIG_TARGET_socfpga_SOCFPGA_SOCKIT),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot
   ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_UIMAGE)$(CONFIG_TARGET_socfpga_SOCFPGA_SOCKIT),)
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-uImage $(TARGET_DIR)/boot/
	ln -sf $(IMG_PREFIX)-uImage $(TARGET_DIR)/boot/uImage
   endif
   ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_ZIMAGE),)
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-zImage $(TARGET_DIR)/boot/
	ln -sf $(IMG_PREFIX)-zImage $(TARGET_DIR)/boot/zImage
   endif
   ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_FIT),)
	$(foreach dts,$(shell echo $(1)),
		$(CP) $(BIN_DIR)/$(IMG_PREFIX)-$(dts)-fit-uImage.itb $(TARGET_DIR)/boot/
	)
   endif
 endif

 ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_DTB)$(CONFIG_TARGET_socfpga_SOCFPGA_SOCKIT),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot
	$(foreach dts,$(shell echo $(1)),
		$(CP) $(BIN_DIR)/$(IMG_PREFIX)-$(dts).dtb $(TARGET_DIR)/boot/,
		ln -sf $(IMG_PREFIX)-$(dts).dtb $(TARGET_DIR)/boot/$(dts).dtb
	)
 endif
endef

define Image/Build/SDCard

 ifeq ($(1),ext4)
	./gen_socfpga_sdcard_img.sh \
		$(BIN_DIR)/$(IMG_PREFIX)-$(call sanitize_profile_name)-sdcard-vfat-$(1).img \
		$(KDIR)/root.$(1) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) \
		$(BIN_DIR)/uboot-socfpga-$(2)/$(IMG_PREFIX)-$(2)-u-boot-with-spl.sfp \
		$(KDIR)/cfg.img
 endif
endef

define Image/mkfs/targz
	$(TAR) -czpf $(BIN_DIR)/$(IMG_PREFIX)-$(call sanitize_profile_name)-rootfs.tar.gz --numeric-owner --owner=0 --group=0 -C $(TARGET_DIR)/ .
endef

Image/BuildKernel/Template/Generic=$(call Image/BuildKernel/Template)
Image/InstallKernel/Template/Generic=$(call Image/InstallKernel/Template)

Image/BuildKernel/Template/SOCFPGA_SOCKIT=$(foreach dts,$(shell echo $(SOCFPGA_SOCKIT_DTS)),$(call Image/BuildKernel/Template,$(dts)))
Image/InstallKernel/Template/SOCFPGA_SOCKIT=$(call Image/InstallKernel/Template,$(SOCFPGA_SOCKIT_DTS))

define Image/BuildKernel
	$(call Image/BuildKernel/Template/$(PROFILE))
endef

define Image/InstallKernel
	$(call Image/InstallKernel/Template/$(PROFILE))
endef

define Image/Build/Profile/SOCFPGA_SOCKIT

 ifeq ($(1),ext4)
	$(call Image/Build/SDCard,$(1),socfpga_cyclone5_sockit)
	$(call Image/Build/SysupgradeNAND,$(call sanitize_profile_name),ext4,)
 endif
 ifeq ($(1),ubifs)
	$(call Image/Build/SysupgradeNAND,$(call sanitize_profile_name),ubifs,)
 endif
endef

define Image/Build
	$(if $(Image/Build/$(1)), \
		$(call Image/Build/$(1),$(1)), \
		$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(call sanitize_profile_name)-$(1).img \
	)

	$(if $(Image/Build/Profile/$(PROFILE)), \
		$(call Image/Build/Profile/$(PROFILE),$(1)), \
		$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(call sanitize_profile_name)-$(1).img \
	)
endef

$(eval $(call BuildImage))
