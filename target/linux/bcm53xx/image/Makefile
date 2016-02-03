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

	# For UBI we want only one extra block
	rm -f $(KDIR)/ubi_mark
	echo -ne '\xde\xad\xc0\xde' > $(KDIR)/ubi_mark

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

define Build/trx-serial
	$(STAGING_DIR_HOST)/bin/trx \
		-o $@ \
		-f $(word 1,$^) -a 1024 \
		-f $(KDIR)/root.squashfs -a 0x10000 -A $(KDIR)/fs_mark
endef

define Build/trx-nand
	# kernel: always use 4 MiB (-28 B or TRX header) to allow upgrades even
	#	  if it grows up between releases
	# root: UBI with one extra block containing UBI mark to trigger erasing
	#	rest of partition
	$(STAGING_DIR_HOST)/bin/trx \
		-o $@ \
		-m 33554432 \
		-f $(word 1,$^) -a 0x20000 -b 0x400000 \
		-f $(KDIR)/root-block-0x20000-min-0x800.ubi \
		-A $(KDIR)/ubi_mark -a 0x20000
endef

define Build/asus-trx
	$(STAGING_DIR_HOST)/bin/asustrx \
		-p $(PRODUCTID) -i $@ -o $@.new
	mv $@.new $@
endef

define Build/seama-nand
	# Seama entity
	$(STAGING_DIR_HOST)/bin/oseama \
		entity $@.entity \
		-m "dev=/dev/mtdblock/7" \
		-m "type=firmware" \
		-f $(word 1,$^) \
		-b 0x400000 \
		-f $(KDIR)/root-block-0x20000-min-0x800.ubi \
		-f $(KDIR)/ubi_mark
	# Seama container
	$(STAGING_DIR_HOST)/bin/seama \
		-s $@ \
		-m "signature=$(SIGNATURE)" \
		-i $@.entity
endef

DEVICE_VARS += DT PRODUCTID SIGNATURE NETGEAR_BOARD_ID NETGEAR_REGION

define Device/Default
  # .dtb files are prefixed by SoC type, e.g. bcm4708- which is not included in device/image names
  # extract the full dtb name based on the device info
  DT := $(patsubst %.dtb,%,$(notdir $(wildcard $(if $(IB),$(KDIR),$(DTS_DIR))/*-$(1).dtb)))
  KERNEL := kernel-bin | append-dtb | lzma-d16
  KERNEL_DEPENDS = $$(wildcard $(KDIR)/$$(DT).dts)
  FILESYSTEMS := squashfs
  KERNEL_NAME := zImage
  IMAGE_NAME = $$(IMAGE_PREFIX)-$$(1).$$(2)
  IMAGES := trx
  IMAGE/trx := trx-nand
endef

define Device/asus
  IMAGES := trx
  IMAGE/trx := trx-nand | asus-trx
endef

define AsusDevice
  define Device/asus-$(1)
	$$(Device/asus)
	PRODUCTID := $(2)
  endef
  TARGET_DEVICES += asus-$(1)
endef

define Device/linksys
  IMAGES := trx
  IMAGE/trx := trx-nand
endef

define LinksysDevice
  define Device/linksys-$(1)
	$$(Device/linksys)
  endef
  TARGET_DEVICES += linksys-$(1)
endef

define Device/dlink
  IMAGES := bin
  IMAGE/bin := seama-nand
endef

define DLinkDevice
  define Device/dlink-$(1)
	$$(Device/dlink)
	SIGNATURE := $(2)
  endef
  TARGET_DEVICES += dlink-$(1)
endef

define Device/netgear
  IMAGES := chk
  IMAGE/chk := trx-nand | netgear-chk
  NETGEAR_REGION := 1
endef

define NetgearDevice
  define Device/netgear-$(1)
	$$(Device/netgear)
	NETGEAR_BOARD_ID := $(2)
  endef
  TARGET_DEVICES += netgear-$(1)
endef

define Device/netgear-r6250
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H245T00_NETGEAR
endef

define Device/netgear-r6300-v2
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H240T00_NETGEAR
endef

define Device/netgear-r8000
  $(Device/netgear)
  NETGEAR_BOARD_ID := U12H315T00_NETGEAR
endef

define Device/smartrg-sr400ac
  IMAGES := trx
  IMAGE/trx := trx-serial
endef

TARGET_DEVICES += \
	buffalo-wzr-1750dhp buffalo-wzr-600dhp2 buffalo-wzr-900dhp \
	buffalo-wxr-1900dhp \
	netgear-r6250 netgear-r6300-v2 netgear-r8000 \
	smartrg-sr400ac

$(eval $(call AsusDevice,rt-ac56u,RT-AC56U))
$(eval $(call AsusDevice,rt-ac68u,RT-AC68U))
$(eval $(call AsusDevice,rt-ac87u,RT-AC87U))
$(eval $(call AsusDevice,rt-n18u,RT-N18U))
$(eval $(call DLinkDevice,dir-885l,wrgac42_dlink.2015_dir885l))
$(eval $(call LinksysDevice,ea6300-v1))
$(eval $(call NetgearDevice,r7000,U12H270T00_NETGEAR))
# $(eval $(call NetgearDevice,r8500,U12H334T00_NETGEAR))

$(eval $(call BuildImage))
