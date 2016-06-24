#
# Copyright (C) 2010-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

# boards missing since devicetree update
#EASY50712 ARV3527P

JFFS2_BLOCKSIZE = 64k 128k 256k

KERNEL_LOADADDR = 0x80002000
KERNEL_ENTRY = 0x80002000

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

LOADER_MAKE := $(NO_TRACE_MAKE) -C lzma-loader KDIR=$(KDIR)

define Image/BuildLoader/Template
	-rm -rf $(KDIR)/lzma-loader
	$(LOADER_MAKE) LOADER=loader$(2)-$(1).bin\
		LZMA_TEXT_START=0x80a00000 \
		LOADADDR=0x80002000 \
		LOADER_DATA="$(KDIR)/vmlinux$(2)-$(1).lzma" BOARD="$(1)" \
		compile loader.bin
endef


define CompressLzma
	$(STAGING_DIR_HOST)/bin/lzma e $(1) $(2)
endef

define PatchKernelLzma
	cp $(KDIR)/vmlinux$(2) $(KDIR)/vmlinux$(2)-$(1)
	$(LINUX_DIR)/scripts/dtc/dtc -O dtb -o $(KDIR)/$(1).dtb ../dts/$(1).dts
	cat $(KDIR)/vmlinux$(2)-$(1) $(KDIR)/$(1).dtb > $(KDIR)/vmlinux$(2)-$(1).tmp
	$(call CompressLzma,$(KDIR)/vmlinux$(2)-$(1).tmp,$(KDIR)/vmlinux$(2)-$(1).lzma)
endef

define MkImageLzma
	mkimage -A mips -O linux -T kernel -a 0x80002000 -C lzma \
		-e 0x80002000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux$(2)-$(1).lzma $(KDIR)/uImage-$(1)$(2)
endef

define MkImageEVA
	lzma2eva 0x80002000 0x80002000 $(KDIR)/vmlinux$(2)-$(1).lzma $(KDIR)/$(1)$(2).eva.prealign
	dd if=$(KDIR)/$(1)$(2).eva.prealign of=$(KDIR)/$(1)$(2).eva.align.64k bs=64k conv=sync
	dd if=$(KDIR)/$(1)$(2).eva.prealign of=$(KDIR)/$(1)$(2).eva.align.128k bs=128k conv=sync
	cat ./eva.dummy.squashfs >> $(KDIR)/$(1)$(2).eva.align.64k
endef

define Image/Build/squashfs
	cat $(KDIR)/uImage-$(2) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image)
endef

define Image/BuildNAND/squashfs

ifneq ($($(PROFILE)_UBI_OPTS),)
	$(call prepare_generic_squashfs,$(KDIR)/root.$(1))
	$(call Image/Build/UbinizeImage,$(PROFILE),,squashfs,$($(PROFILE)_UBI_OPTS))
	$(call Image/Build/SysupgradeNAND,$(PROFILE),$(1),$(KDIR)/uImage-$(PROFILE))
endif
endef

DGN3500_SKERNEL=0x50000
DGN3500_SKERNEL_DECIMAL=327680
define Image/BuildDGN3500/squashfs
	dd if=/dev/zero of=$(BIN_DIR)/$(IMG_PREFIX)-pad bs=$(DGN3500_SKERNEL_DECIMAL) count=1
	cat $(BIN_DIR)/$(IMG_PREFIX)-pad $(KDIR)/uImage-$(2) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img
	rm -r $(BIN_DIR)/$(IMG_PREFIX)-pad
	dd if=/dev/zero ibs=16M count=1 | tr "\000" "\377" > $(BIN_DIR)/$(IMG_PREFIX)-pwf
	cp $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadNA.img
	dgn3500sum $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadNA.img $(DGN3500_SKERNEL) NA
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadNA.img)
	cp $(BIN_DIR)/$(IMG_PREFIX)-pwf $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-NA.img
	dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadNA.img of=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-NA.img conv=notrunc
	dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadNA.img of=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-sysupgrade-NA.image bs=$(DGN3500_SKERNEL_DECIMAL) skip=1
	rm -r $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadNA.img
	mv $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadWW.img
	dgn3500sum $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadWW.img $(DGN3500_SKERNEL) WW
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadWW.img)
	mv $(BIN_DIR)/$(IMG_PREFIX)-pwf $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-WW.img
	dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadWW.img of=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-WW.img conv=notrunc
	dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadWW.img of=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-sysupgrade-WW.image bs=$(DGN3500_SKERNEL_DECIMAL) skip=1
	rm -r $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepadWW.img
endef

define Image/BuildDGN3500B/squashfs
	dd if=/dev/zero of=$(BIN_DIR)/$(IMG_PREFIX)-pad bs=327680 count=1
	cat $(BIN_DIR)/$(IMG_PREFIX)-pad $(KDIR)/uImage-$(2) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img
	rm -r $(BIN_DIR)/$(IMG_PREFIX)-pad
	dd if=/dev/zero ibs=16M count=1 | tr "\000" "\377" > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory.img
	dgn3500sum $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img $(DGN3500_SKERNEL) DE
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img)
	dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img of=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory.img conv=notrunc
	dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img of=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-sysupgrade.image bs=$(DGN3500_SKERNEL_DECIMAL) skip=1
	rm -r $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-factory-prepad.img
	dgn3500sum $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)-sysupgrade.image $(DGN3500_SKERNEL) DE
endef


define Image/BuildEVA/squashfs
	cat $(KDIR)/$(2).eva.align.64k $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image.eva
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image.eva)
endef

define Image/BuildEVA/ubifs

ifneq ($($(PROFILE)_UBIFS_OPTS),)
	$(CP) $(KDIR)/root.ubifs $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs.ubifs
endif
endef

define Image/BuildEVA/ubi

ifneq ($($(PROFILE)_UBI_OPTS),)
	$(CP) $(KDIR)/root.ubi $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs.ubi
	$(CP) $(KDIR)/root-overlay.ubi $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs-overlay.ubi
endif
endef

define Image/BuildLoader/squashfs
	dd if=$(KDIR)/loader-$(2).bin of=$(KDIR)/loader-$(2).bin.padded bs=3072k conv=sync
	cat $(KDIR)/loader-$(2).bin.padded $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image
	$(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image)
endef

define Image/BuildEVA/jffs2-128k
	cat $(KDIR)/$(2).eva.align.128k $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image.eva
endef

define Image/Build/jffs2-64k
	dd if=$(KDIR)/uImage-$(2) of=$(KDIR)/uImage-$(2)-$(1) bs=64k conv=sync
	cat $(KDIR)/uImage-$(2)-$(1) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image
endef

define Image/Build/jffs2-128k
	dd if=$(KDIR)/uImage-$(2) of=$(KDIR)/uImage-$(2)-$(1) bs=128k conv=sync
	cat $(KDIR)/uImage-$(2)-$(1) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image
endef

define Image/Build/jffs2-256k
	dd if=$(KDIR)/uImage-$(2) of=$(KDIR)/uImage-$(2)-$(1) bs=256k conv=sync
	cat $(KDIR)/uImage-$(2)-$(1) $(KDIR)/root.$(1) > $(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1).image
endef

define Image/Build/ubifs

ifneq ($($(PROFILE)_UBIFS_OPTS),)
	$(CP) $(KDIR)/root.ubifs $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs.ubifs
endif
endef

define Image/Build/ubi

ifneq ($($(PROFILE)_UBI_OPTS),)
	$(CP) $(KDIR)/root.ubi $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs.ubi
	$(CP) $(KDIR)/root-overlay.ubi $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-rootfs-overlay.ubi
endif
endef


define Image/BuildNAND/ubifs

ifneq ($($(PROFILE)_UBIFS_OPTS),)
ifneq ($($(PROFILE)_UBI_OPTS),)
	$(call Image/Build/UbinizeImage,$(PROFILE),,ubifs,$($(PROFILE)_UBI_OPTS))
	$(call Image/Build/SysupgradeNAND,$(PROFILE),$(1),$(KDIR)/uImage-$(PROFILE))
endif
endif
endef


define Image/InstallKernel/Template

ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_KERNEL),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot

ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_UIMAGE),)
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage $(TARGET_DIR)/boot/
	ln -sf $(IMG_PREFIX)-$(1)-uImage $(TARGET_DIR)/boot/uImage
endif

ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_ZIMAGE),)
	$(CP) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-zImage $(TARGET_DIR)/boot/
	ln -sf $(IMG_PREFIX)-$(1)-zImage $(TARGET_DIR)/boot/zImage
endif
endif

ifneq ($(CONFIG_TARGET_ROOTFS_INCLUDE_DTB),)
	$(INSTALL_DIR) $(TARGET_DIR)/boot

ifneq ($(1),)
	$(CP) $(KDIR)/$(1).dtb $(TARGET_DIR)/boot/
endif
endif
endef

define Image/BuildKernel/Template
	$(call PatchKernelLzma,$(1))
	$(call MkImageLzma,$(1))
	$(CP) $(KDIR)/uImage-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call PatchKernelLzma,$(1),-initramfs)
	$(call MkImageLzma,$(1),-initramfs)
	$(CP) $(KDIR)/uImage-$(1)-initramfs $(BIN_DIR)/$(IMG_PREFIX)-$(1)-uImage-initramfs
endif
endef

define Image/BuildKernelLoader/Template
	$(CP) $(KDIR)/loader-$(1).bin $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux-loader
	$(CP) $(KDIR)/vmlinux-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(CP) $(KDIR)/loader-initramfs-$(1).bin $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux-initramfs-loader
	$(CP) $(KDIR)/vmlinux-initramfs-$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-vmlinux-initramfs
endif
endef

define Image/Prepare/Profile
	$(call PatchKernelLzma,$(1))
	$(call Image/BuildLoader/Template,$(1))
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call PatchKernelLzma,$(1),-initramfs)
	$(call Image/BuildLoader/Template,$(1),-initramfs)
endif
endef

define Image/BuildKernelEVA/Template
	$(call PatchKernelLzma,$(1))
	$(call MkImageEVA,$(1))
	$(CP) $(KDIR)/$(1).eva.align.64k $(BIN_DIR)/$(IMG_PREFIX)-$(1).eva.align.64k
	$(CP) $(KDIR)/$(1).eva.align.128k $(BIN_DIR)/$(IMG_PREFIX)-$(1).eva.align.128k
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call PatchKernelLzma,$(1),-initramfs)
	$(call MkImageEVA,$(1),-initramfs)
	$(CP) $(KDIR)/$(1)-initramfs.eva.align.64k $(BIN_DIR)/$(IMG_PREFIX)-$(1)-initramfs.eva.align.64k
	$(CP) $(KDIR)/$(1)-initramfs.eva.align.128k $(BIN_DIR)/$(IMG_PREFIX)-$(1)-initramfs.eva.align.128k
endif
endef


### Kernel scripts ###
define Build/append-dtb
	$(call Image/BuildDTB,../dts/$(DEVICE_DTS).dts,$@.dtb)
	cat $@.dtb >> $@
endef

define Build/mkbrncmdline
   mkbrncmdline -i $@ -o $@.new BRN-BOOT
   mv $@.new $@
endef

define Build/mkbrnimg
   mkbrnimg -s $(SIGNATURE) -m $(MAGIC) -p $(CRC32_POLY) -o $@ $(word 1,$^) $(word 2,$^)
endef

# Shared device definition: applies to every defined device
define Device/Default
  PROFILES = Default $$(DEVICE_PROFILE)
  KERNEL_DEPENDS = $$(wildcard ../dts/$$(DEVICE_DTS).dts)
  KERNEL_INITRAMFS_NAME = $$(KERNEL_NAME)-initramfs
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | uImage lzma
  FILESYSTEMS := squashfs
  DEVICE_PROFILE :=
  DEVICE_DTS = $$(DEVICE_PROFILE)
  IMAGE_SIZE :=
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | check-size $$$$(IMAGE_SIZE)
endef
DEVICE_VARS += DEVICE_PROFILE DEVICE_DTS IMAGE_SIZE

define Device/lantiqBrnImage
  KERNEL := kernel-bin | append-dtb | mkbrncmdline | lzma-no-dict
  IMAGES := factory.bin
  IMAGE/factory.bin := mkbrnimg | check-size $$$$(IMAGE_SIZE)
endef
DEVICE_VARS += SIGNATURE MAGIC CRC32_POLY

ifeq ($(SUBTARGET),xway_legacy)

define Device/ARV4520PW
  DEVICE_PROFILE := ARV4520PW
  IMAGE_SIZE := 3648k
  DEVICE_TITLE := Easybox 800, WAV-281 - ARV4520PW
  DEVICE_PACKAGES := kmod-ltq-hcd-danube \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-rt61-pci wpad-mini \
	swconfig
endef
TARGET_DEVICES += ARV4520PW

define Device/ARV4525PW
  DEVICE_PROFILE := ARV4525PW
  IMAGE_SIZE := 3776k
  DEVICE_TITLE := Speedport W502V Typ A - ARV4525PW
  DEVICE_PACKAGES := kmod-ath5k wpad-mini \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa
endef
TARGET_DEVICES += ARV4525PW

define Device/ARV452CQW
  DEVICE_PROFILE := ARV452CQW
  IMAGE_SIZE := 3776k
  DEVICE_TITLE := Easybox 801 - ARV452CQW
  DEVICE_PACKAGES := kmod-ltq-hcd-danube \
	kmod-ath5k wpad-mini \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	swconfig
endef
TARGET_DEVICES += ARV452CQW

define Device/ARV4518PWR01
  DEVICE_PROFILE := ARV4518PWR01
  IMAGE_SIZE := 3776k
  DEVICE_TITLE := ARV4518PWR01
  DEVICE_PACKAGES := kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath5k wpad-mini \
	swconfig
endef
TARGET_DEVICES += ARV4518PWR01

define Device/ARV4518PWR01A
  DEVICE_PROFILE := ARV4518PWR01A
  IMAGE_SIZE := 3776k
  DEVICE_TITLE := ARV4518PWR01A
  DEVICE_PACKAGES := kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath5k wpad-mini \
	swconfig
endef
TARGET_DEVICES += ARV4518PWR01A

endif

ifeq ($(SUBTARGET),xway)

# Danube
BTHOMEHUBV2B_UBIFS_OPTS:="-m 512 -e 15872 -c 1959"
BTHOMEHUBV2B_UBI_OPTS:="-m 512 -p 16KiB -s 256"

Image/BuildKernel/Profile/BTHOMEHUBV2B=$(call Image/BuildKernel/Template,BTHOMEHUBV2B)
Image/Build/Profile/BTHOMEHUBV2B=$(call Image/BuildNAND/$(1),$(1),BTHOMEHUBV2B)

define Device/EASY50712
  DEVICE_PROFILE := EASY50712
  IMAGE_SIZE := 3776k
  DEVICE_TITLE := Lantiq Danube - EASY50712
endef
TARGET_DEVICES += EASY50712

define Device/ACMP252
  DEVICE_PROFILE := ACMP252
  IMAGE_SIZE := 14848k
  DEVICE_TITLE := AudioCodes MediaPack MP-252
  DEVICE_PACKAGES := kmod-rt2x00-pci wpad-mini \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-ptm-danube \
	ltq-adsl-app
endef
TARGET_DEVICES += ACMP252

define Device/ARV4510PW
  DEVICE_PROFILE := ARV4510PW
  IMAGE_SIZE := 15616k
  DEVICE_TITLE := Wippies, Elisa - ARV4510PW
  DEVICE_PACKAGES := kmod-ledtrig-usbdev kmod-usb2-pci kmod-usb-uhci \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-tapi kmod-ltq-vmmc \
	kmod-rt2800-pci kmod-ath5k wpad-mini \
	swconfig
endef
TARGET_DEVICES += ARV4510PW

define Device/ARV7525PW
  DEVICE_PROFILE := ARV7525PW
  IMAGE_SIZE := 3776k
  DEVICE_TITLE := Speedport W303V Typ A - ARV7525PW
  DEVICE_PACKAGES := kmod-rt2800-pci wpad-mini \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa
endef
TARGET_DEVICES += ARV7525PW

define Device/ARV4519PW
  DEVICE_PROFILE := ARV4519PW
  IMAGE_SIZE := 3776k
  DEVICE_TITLE := Vodafone, Pirelli - ARV4519PW
  DEVICE_PACKAGES := kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	swconfig
endef
TARGET_DEVICES += ARV4519PW

define Device/ARV7510PW22
  DEVICE_PROFILE := ARV7510PW22
  IMAGE_SIZE := 31232k
  DEVICE_TITLE := Astoria - ARV7510PW22
  DEVICE_PACKAGES := kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-tapi kmod-ltq-vmmc \
	kmod-rt2800-pci wpad-mini \
	kmod-usb-uhci kmod-usb2 kmod-usb2-pci \
	swconfig
endef
TARGET_DEVICES += ARV7510PW22

define Device/ARV7518PW
  DEVICE_PROFILE := ARV7518PW
  IMAGE_SIZE := 7872k
  DEVICE_TITLE := Astoria - ARV7518PW
  DEVICE_PACKAGES := kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath9k wpad-mini \
	swconfig
endef
TARGET_DEVICES += ARV7518PW

define Device/ARV7519PW
  DEVICE_PROFILE := ARV7519PW
  IMAGE_SIZE := 15488k
  DEVICE_TITLE := Astoria - ARV7519PW
  DEVICE_PACKAGES := kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-rt2800-pci wpad-mini \
	swconfig
endef
TARGET_DEVICES += ARV7519PW

define Device/ARV752DPW
  DEVICE_PROFILE := ARV752DPW
  IMAGE_SIZE := 7872k
  DEVICE_TITLE := Easybox 802 - ARV752DPW
  DEVICE_PACKAGES := kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-tapi kmod-ltq-vmmc \
	kmod-rt2800-pci wpad-mini \
	swconfig
endef
TARGET_DEVICES += ARV752DPW

define Device/ARV752DPW22
  DEVICE_PROFILE := ARV752DPW22
  IMAGE_SIZE := 7616k
  DEVICE_TITLE := Easybox 803 - ARV752DPW22
  DEVICE_PACKAGES := kmod-usb2-pci kmod-usb-uhci kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-tapi kmod-ltq-vmmc \
	kmod-rt2800-pci wpad-mini \
	swconfig
endef
TARGET_DEVICES += ARV752DPW22

define Device/ARV8539PW22
  DEVICE_PROFILE := ARV8539PW22
  IMAGE_SIZE := 7616k
  DEVICE_TITLE := Speedport W504V Typ A - ARV8539PW22
  DEVICE_PACKAGES := kmod-ltq-hcd-danube \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath9k wpad-mini \
	swconfig
endef
TARGET_DEVICES += ARV8539PW22

define Device/GIGASX76X
  DEVICE_PROFILE := GIGASX76X
  IMAGE_SIZE := 7680k
  DEVICE_TITLE := Gigaset sx76x
  DEVICE_PACKAGES := kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoe \
	kmod-ath5k wpad-mini \
	swconfig
endef
TARGET_DEVICES += GIGASX76X



# AR9
BTHOMEHUBV3A_UBIFS_OPTS:="-m 512 -e 15872 -c 1959"
BTHOMEHUBV3A_UBI_OPTS:="-m 512 -p 16KiB -s 256"

Image/BuildKernel/Profile/BTHOMEHUBV3A=$(call Image/BuildKernel/Template,BTHOMEHUBV3A)
Image/Build/Profile/BTHOMEHUBV3A=$(call Image/BuildNAND/$(1),$(1),BTHOMEHUBV3A)

Image/BuildKernel/Profile/DGN3500=$(call Image/BuildKernel/Template,DGN3500)
Image/Build/Profile/DGN3500=$(call Image/BuildDGN3500/$(1),$(1),DGN3500)

Image/BuildKernel/Profile/DGN3500B=$(call Image/BuildKernel/Template,DGN3500B)
Image/Build/Profile/DGN3500B=$(call Image/BuildDGN3500B/$(1),$(1),DGN3500B)

define Device/WBMRA
  DEVICE_PROFILE := WBMRA
  IMAGE_SIZE := 31488k
  DEVICE_DTS := WBMR
  DEVICE_TITLE := Buffalo WBMR-HP-G300H (A) - WBMR
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-ledtrig-usbdev \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-a kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath9k wpad hostapd-utils \
	swconfig
endef
TARGET_DEVICES += WBMRA

define Device/WBMRB
  DEVICE_PROFILE := WBMRB
  IMAGE_SIZE := 31488k
  DEVICE_DTS := WBMR
  DEVICE_TITLE := Buffalo WBMR-HP-G300H (B) - WBMR
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-ledtrig-usbdev \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath9k wpad hostapd-utils \
	swconfig
endef
TARGET_DEVICES += WBMRB


Image/BuildKernel/Profile/FRITZ7320=$(call Image/BuildKernelEVA/Template,FRITZ7320)
Image/Build/Profile/FRITZ7320=$(call Image/BuildEVA/$(1),$(1),FRITZ7320)

define Device/GR7000
  DEVICE_PROFILE := GR7000
  IMAGE_SIZE := 7808k
  DEVICE_TITLE := Aztech GR7000
  DEVICE_PACKAGES := kmod-ltq-deu-ar9 kmod-usb-dwc2 swconfig
endef
TARGET_DEVICES += GR7000

define Device/H201L
  DEVICE_PROFILE := H201L
  IMAGE_SIZE := 7808k
  DEVICE_TITLE := ZTE H201L - H201L
  DEVICE_PACKAGES := kmod-ath9k-htc wpad-mini \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoe \
	kmod-ltq-deu-ar9 kmod-usb-dwc2 \
	kmod-ltq-tapi kmod-ltq-vmmc \
	swconfig
endef
TARGET_DEVICES += H201L

define Device/P2601HNFX
  DEVICE_PROFILE := P2601HNFX
  IMAGE_SIZE := 15616k
  DEVICE_TITLE := ZyXEL P-2601HN-Fx
  DEVICE_PACKAGES := kmod-rt2800-usb wpad-mini \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoe \
	kmod-ltq-deu-ar9 kmod-usb-dwc2 \
	swconfig
endef
TARGET_DEVICES += P2601HNFX


endif


ifeq ($(SUBTARGET),xrx200)

# VR9
P2812HNUF1_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
P2812HNUF1_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

Image/BuildKernel/Profile/P2812HNUF1=$(call Image/BuildKernel/Template,P2812HNUF1)
Image/Build/Profile/P2812HNUF1=$(call Image/BuildNAND/$(1),$(1),P2812HNUF1)

P2812HNUF3_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
P2812HNUF3_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

Image/BuildKernel/Profile/P2812HNUF3=$(call Image/BuildKernel/Template,P2812HNUF3)
Image/Build/Profile/P2812HNUF3=$(call Image/BuildNAND/$(1),$(1),P2812HNUF3)

define Device/ARV7519RW22
  DEVICE_PROFILE := ARV7519RW22
  IMAGE_SIZE := 31232k
  DEVICE_TITLE := Livebox Astoria ARV7519RW22
  DEVICE_PACKAGES := kmod-usb-dwc2
endef
TARGET_DEVICES += ARV7519RW22


BTHOMEHUBV5A_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
BTHOMEHUBV5A_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

Image/BuildKernel/Profile/BTHOMEHUBV5A=$(call Image/BuildKernel/Template,BTHOMEHUBV5A)
Image/Build/Profile/BTHOMEHUBV5A=$(call Image/BuildNAND/$(1),$(1),BTHOMEHUBV5A)

define Device/EASY80920NAND
  DEVICE_PROFILE := EASY80920NAND
  IMAGE_SIZE := 64512k
  DEVICE_TITLE := Lantiq VR9 - EASY80920NAND
  DEVICE_PACKAGES := kmod-ath9k wpad-mini kmod-usb-dwc2
endef
TARGET_DEVICES += EASY80920NAND

define Device/EASY80920NOR
  DEVICE_PROFILE := EASY80920NOR
  IMAGE_SIZE := 7936k
  DEVICE_TITLE := Lantiq VR9 - EASY80920NOR
  DEVICE_PACKAGES := kmod-ath9k wpad-mini kmod-usb-dwc2
endef
TARGET_DEVICES += EASY80920NOR


FRITZ3370_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
FRITZ3370_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

Image/BuildKernel/Profile/FRITZ3370=$(call Image/BuildKernelEVA/Template,FRITZ3370)
Image/Build/Profile/FRITZ3370=$(call Image/BuildEVA/$(1),$(1),FRITZ3370)

Image/BuildKernel/Profile/FRITZ7360SL=$(call Image/BuildKernelEVA/Template,FRITZ7360SL)
Image/Build/Profile/FRITZ7360SL=$(call Image/BuildEVA/$(1),$(1),FRITZ7360SL)

Image/BuildKernel/Profile/VG3503J=$(call Image/BuildKernelLoader/Template,VG3503J)
Image/Build/Profile/VG3503J=$(call Image/BuildLoader/$(1),$(1),VG3503J)

include tp-link.mk

define Device/WBMR300
  DEVICE_PROFILE := WBMR300
  IMAGE_SIZE := 15616k
  DEVICE_TITLE := Buffalo WBMR-300HPD
  DEVICE_PACKAGES := kmod-mt76 wpad-mini kmod-usb-dwc2
endef
TARGET_DEVICES += WBMR300

define Device/VGV7510KW22NOR
  DEVICE_PROFILE := VGV7510KW22NOR
  IMAGE_SIZE := 15232k
  DEVICE_TITLE := o2 Box 6431 / Arcadyan VGV7510KW22 (NOR)
  DEVICE_PACKAGES := kmod-rt2800-pci wpad-mini kmod-usb-dwc2
endef
TARGET_DEVICES += VGV7510KW22NOR

define Device/VGV7510KW22BRN
  $(Device/lantiqBrnImage)
  DEVICE_PROFILE := VGV7510KW22BRN
  IMAGE_SIZE := 7168k
  SIGNATURE := 5D00008000
  MAGIC := 0x12345678
  CRC32_POLY := 0x2083b8ed
  DEVICE_TITLE := o2 Box 6431 / Arcadyan VGV7510KW22 (BRN)
  DEVICE_PACKAGES := kmod-rt2800-pci wpad-mini kmod-usb-dwc2
endef
TARGET_DEVICES += VGV7510KW22BRN

define Device/VGV7519NOR
  DEVICE_PROFILE := VGV7519NOR
  IMAGE_SIZE := 15360k
  DEVICE_TITLE := Experiabox 8 VGV7519
  DEVICE_PACKAGES := kmod-rt2800-pci wpad-mini kmod-usb-dwc2
endef
TARGET_DEVICES += VGV7519NOR

define Device/VGV7519BRN
  $(Device/lantiqBrnImage)
  DEVICE_PROFILE := VGV7519BRN
  IMAGE_SIZE := 7168k
  SIGNATURE := 5D00008000
  MAGIC := 0x12345678
  CRC32_POLY := 0x2083b8ed
  DEVICE_TITLE := Experiabox 8 VGV7519 (BRN)
  DEVICE_PACKAGES := kmod-rt2800-pci wpad-mini kmod-usb-dwc2
endef
TARGET_DEVICES += VGV7519BRN

define Image/Prepare
	$(call Image/Prepare/Profile,VG3503J)
endef

endif

define Image/BuildKernel
	$(call Image/BuildKernel/Profile/$(PROFILE))
endef

define Image/InstallKernel
	$(call Image/InstallKernel/Template/$(PROFILE))
endef

define Image/Build
	$(call Image/Build/Profile/$(PROFILE),$(1))
endef

$(eval $(call BuildImage))
