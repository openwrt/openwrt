#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
	$(CP) $(DTS_DIR)/*.dtb $(KDIR)/

	rm -f $(KDIR)/fs_mark
	echo -ne '\xde\xad\xc0\xde' > $(KDIR)/fs_mark
	$(call prepare_generic_squashfs,$(KDIR)/fs_mark)

	$(CP) ./ubinize.cfg $(KDIR)
endef

define Build/append-dtb
	cat $(KDIR)/$(DT).dtb >> $@
endef

define Build/lzma-d16
	$(STAGING_DIR_HOST)/bin/lzma e $@ -d16 $(1) $@.new
	@mv $@.new $@
endef

define Build/mkfs/squashfs
	( cd $(KDIR); $(STAGING_DIR_HOST)/bin/ubinize -p 128KiB -m 2048 -o $(KDIR)/root-block-0x20000-min-0x800.ubi ubinize.cfg )
endef

define Build/trx-nand
	$(STAGING_DIR_HOST)/bin/trx \
		-o $@ \
		-f $(word 1,$^) \
		-a 0x20000 -f $(KDIR)/root-block-0x20000-min-0x800.ubi \
		-a 0x20000 -A $(KDIR)/fs_mark
endef

define Build/netgear-chk
	$(STAGING_DIR_HOST)/bin/mkchkimg \
		-o $@.new -k $@ -b $(BOARD_ID) -r $(REGION)
	mv $@.new $@
endef

DEVICE_VARS += DT BOARD_ID REGION

define Device/Default
  # .dtb files are prefixed by SoC type, e.g. bcm4708- which is not included in device/image names
  # extract the full dtb name based on the device info
  DT := $(patsubst %.dtb,%,$(notdir $(wildcard $(if $(IB),$(KDIR),$(DTS_DIR))/*-$(1).dtb)))
  KERNEL := kernel-bin | append-dtb | lzma-d16
  FILESYSTEMS := squashfs
  IMAGE_NAME = $$(IMAGE_PREFIX)-$$(1).$$(2)
  IMAGES := trx
  IMAGE/trx := trx-nand
endef

define Device/netgear
  IMAGES := chk
  IMAGE/chk := trx-nand | netgear-chk
  REGION := 1
endef

define Device/netgear-r6250
  $(Device/netgear)
  BOARD_ID := U12H245T00_NETGEAR
endef

define Device/netgear-r6300-v2
  $(Device/netgear)
  BOARD_ID := U12H240T00_NETGEAR
endef

define Device/netgear-r8000
  $(Device/netgear)
  BOARD_ID := U12H315T00_NETGEAR
endef

TARGET_DEVICES += \
	asus-rt-ac68u asus-rt-n18u asus-rt-ac87u \
	netgear-r6250 netgear-r6300-v2 netgear-r8000 \
	buffalo-wzr-1750dhp buffalo-wzr-600dhp2 buffalo-wzr-900dhp

$(eval $(call BuildImage))
