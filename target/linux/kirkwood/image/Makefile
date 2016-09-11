#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

NAND_BLOCKSIZE := 2048-128k

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

KERNEL_LOADADDR:=0x8000
TARGET_DEVICES = linksys-audi linksys-viper dockstar goflexnet goflexhome

UBI_OPTS := -m 2048 -p 128KiB -s 512
UBIFS_OPTS := -m 2048 -e 126KiB -c 4096

define Device/Default
  KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
  KERNEL := kernel-bin | append-dtb | uImage none
  KERNEL_NAME := zImage
  KERNEL_SUFFIX  := -uImage
  KERNEL_INSTALL := 1

  PAGESIZE := 2048
  SUBPAGESIZE := 512
  BLOCKSIZE := 128k
  IMAGES := sysupgrade.tar
  UBINIZE_OPTS := -E 5
  IMAGE/sysupgrade.tar := sysupgrade-tar
endef

define Device/dockstar
  DEVICE_DTS := kirkwood-dockstar
  FILESYSTEMS := squashfs
  PROFILES := Generic DOCKSTAR
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi
  KERNEL_IN_UBI := 1
  KERNEL := kernel-bin | append-dtb
endef

define Device/goflexnet
$(Device/dockstar)
  PROFILES := Generic GOFLEXNET
  DEVICE_DTS := kirkwood-goflexnet
endef

define Device/goflexhome
$(Device/dockstar)
  PROFILES := Generic GOFLEXHOME
  DEVICE_DTS := kirkwood-goflexhome
endef

define Device/linksys-audi
  DEVICE_DTS := kirkwood-linksys-audi
  KERNEL_SIZE := 2624k
  FILESYSTEMS := squashfs
  PROFILES := Generic AUDI
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi
endef

define Device/linksys-viper
  DEVICE_DTS := kirkwood-linksys-viper
  KERNEL_SIZE := 2688k
  FILESYSTEMS := squashfs
  PROFILES := Generic VIPER
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi
endef

define Image/BuildKernel/Template

	$(CP) $(KDIR)/zImage $(BIN_DIR)/$(IMG_PREFIX)-zImage
	echo -ne '\x00\x00\x00\x00' >> $(BIN_DIR)/$(IMG_PREFIX)-zImage
	$(call Image/BuildKernel/MkuImage, \
		none, 0x8000, 0x8000, \
		$(BIN_DIR)/$(IMG_PREFIX)-zImage, \
		$(BIN_DIR)/$(IMG_PREFIX)-uImage \
	)

 ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(CP) $(KDIR)/zImage-initramfs $(BIN_DIR)/$(IMG_PREFIX)-zImage-initramfs
	echo -ne '\x00\x00\x00\x00' >> $(BIN_DIR)/$(IMG_PREFIX)-zImage-initramfs
	$(call Image/BuildKernel/MkuImage, \
		none, 0x8000, 0x8000, \
		$(BIN_DIR)/$(IMG_PREFIX)-zImage-initramfs, \
		$(BIN_DIR)/$(IMG_PREFIX)-uImage-initramfs \
	)
 endif

 ifneq ($(1),)
	$(CP) $(DTS_DIR)/kirkwood-$(1).dtb $(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb

	$(CP) $(KDIR)/zImage $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE_SANITIZED)-zImage
	cat $(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb >> $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE_SANITIZED)-zImage
	$(call Image/BuildKernel/MkuImage, \
		none, 0x8000, 0x8000, \
		$(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE_SANITIZED)-zImage, \
		$(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE_SANITIZED)-uImage \
	)

  ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(CP) $(KDIR)/zImage-initramfs $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE_SANITIZED)-zImage-initramfs
	cat $(BIN_DIR)/$(IMG_PREFIX)-$(1).dtb >> $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE_SANITIZED)-zImage-initramfs
	$(call Image/BuildKernel/MkuImage, \
		none, 0x8000, 0x8000, \
		$(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE_SANITIZED)-zImage-initramfs, \
		$(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE_SANITIZED)-uImage-initramfs \
	)
  endif
 endif
endef

define Image/Build/jffs2-nand-2048-128k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE_SANITIZED)-$(1).img \
		bs=2048 conv=sync
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	( \
		dd if=$(KDIR)/zImage bs=4096k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE_SANITIZED)-$(1).img
endef

define Image/Build/ubifs

 ifneq ($($(PROFILE)_UBIFS_OPTS),)
	$(CP) $(KDIR)/root.ubifs $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE_SANITIZED)-rootfs.ubifs
 endif
endef

define Image/Build/ubi

 ifneq ($($(PROFILE)_UBI_OPTS),)
	$(CP) $(KDIR)/root.ubi $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE_SANITIZED)-rootfs.ubi
 endif
endef

IOMEGA_IX2_200_UBIFS_OPTS := -m 512 -e 15872 -c 2048
IOMEGA_IX2_200_UBI_OPTS := -m 512 -p 16384 -s 256

Image/BuildKernel/Template/Generic=$(call Image/BuildKernel/Template)
Image/BuildKernel/Template/IB62X0=$(call Image/BuildKernel/Template,ib62x0)
Image/BuildKernel/Template/ICONNECT=$(call Image/BuildKernel/Template,iconnect)
Image/BuildKernel/Template/IOMEGA_IX2_200=$(call Image/BuildKernel/Template,iomega_ix2_200)
Image/BuildKernel/Template/NSA310S=$(call Image/BuildKernel/Template,nsa310s)
Image/BuildKernel/Template/POGOE02=$(call Image/BuildKernel/Template,pogo_e02)
Image/BuildKernel/Template/SHEEVAPLUG=$(call Image/BuildKernel/Template,sheevaplug)
Image/BuildKernel/Template/SHEEVAPLUGSATA=$(call Image/BuildKernel/Template,sheevaplug-esata)
Image/BuildKernel/Template/GuruplugServerPlus=$(call Image/BuildKernel/Template,guruplug-server-plus)
Image/BuildKernel/Template/Topkick1281P2=$(call Image/BuildKernel/Template,topkick)

define Image/BuildKernel
	$(call Image/BuildKernel/Template/$(PROFILE))
endef

define Image/InstallKernel
	$(call Image/InstallKernel/Template/$(PROFILE))
endef

define Image/Build
	$(if $(Image/Build/$(1)), \
		$(call Image/Build/$(1),$(1)), \
		$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE_SANITIZED)-$(1).img \
	)
endef

$(eval $(call BuildImage))
