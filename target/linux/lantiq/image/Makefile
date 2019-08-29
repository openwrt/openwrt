#
# Copyright (C) 2010-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

# boards missing since devicetree update
#EASY50712 ARV3527P

KERNEL_LOADADDR = 0x80002000
KERNEL_ENTRY = 0x80002000

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

ifeq ($(SUBTARGET),xway)
  UBIFS_OPTS := -m 512 -e 15872 -c 1959
else
  UBIFS_OPTS := -m 2048 -e 126KiB -c 4096
endif

define Build/append-avm-fakeroot
	cat ./eva.dummy.squashfs >> $@
endef

define Build/dgn3500-sercom-footer
	$(STAGING_DIR_HOST)/bin/dgn3500sum $@ $(1) $(2)
endef

define Build/mkbrncmdline
   mkbrncmdline -i $@ -o $@.new BRN-BOOT
   mv $@.new $@
endef

define Build/mkbrnimg
   mkbrnimg -s $(SIGNATURE) -m $(MAGIC) -p $(CRC32_POLY) -o $@ $(IMAGE_KERNEL) $(IMAGE_ROOTFS)
endef

define Build/fullimage
	mkimage -A mips -O linux -C lzma -T filesystem -a 0x00  \
		-e 0x00 -n '$(VERSION_DIST) RootFS' \
		-d $(IMAGE_ROOTFS) $(IMAGE_ROOTFS).new

	cat $(IMAGE_KERNEL) $(IMAGE_ROOTFS).new > $@.tmp

	mkimage -A mips -O linux -T multi -a 0x00 -C none \
		-e 0x00 -n 'OpenWrt fullimage' \
		-d $@.tmp $@

	rm $(IMAGE_ROOTFS).new
	rm $@.tmp
endef

# Shared device definition: applies to every defined device
define Device/Default
  PROFILES = Default
  KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
  KERNEL_INITRAMFS_NAME = $$(KERNEL_NAME)-initramfs
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | uImage lzma
  FILESYSTEMS := squashfs
  IMAGE_SIZE :=
  SUPPORTED_DEVICES := $(subst _,$(comma),$(1))
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
endef
DEVICE_VARS += IMAGE_SIZE

define Device/NAND/xway
  BLOCKSIZE := 16k
  PAGESIZE := 512
  SUBPAGESIZE := 256
  FILESYSTEMS += ubifs
endef

define Device/NAND/xrx200
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  FILESYSTEMS += ubifs
endef

define Device/NAND
  $(Device/NAND/$(SUBTARGET))
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef

define Device/lantiqBrnImage
  KERNEL := kernel-bin | append-dtb | mkbrncmdline | lzma-no-dict
  IMAGES := factory.bin
  IMAGE/factory.bin := mkbrnimg | check-size $$$$(IMAGE_SIZE)
endef
DEVICE_VARS += SIGNATURE MAGIC CRC32_POLY

define Device/lantiqFullImage
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma | pad-offset 4 0
  IMAGES := sysupgrade.bin fullimage.bin
  IMAGE/fullimage.bin := fullimage | check-size $$$$(IMAGE_SIZE)
endef

define Device/AVM
  DEVICE_VENDOR := AVM
  KERNEL := kernel-bin | append-dtb | lzma | eva-image
  KERNEL_INITRAMFS := $$(KERNEL)
  IMAGE/sysupgrade.bin := append-kernel | pad-to 64k | append-avm-fakeroot | \
	append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
endef

ifeq ($(SUBTARGET),ase)

define Device/allnet_all0333cj
  DEVICE_VENDOR := Allnet
  DEVICE_MODEL := ALL0333CJ
  IMAGE_SIZE := 3700k
  DEVICE_DTS := ALL0333CJ
  DEVICE_PACKAGES := kmod-ltq-adsl-ase kmod-ltq-adsl-ase-mei \
	kmod-ltq-adsl-ase-fw-b kmod-ltq-atm-ase \
	ltq-adsl-app ppp-mod-pppoe
endef
TARGET_DEVICES += allnet_all0333cj

define Device/netgear_dgn1000b
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := DGN1000B
  IMAGE_SIZE := 6000k
  DEVICE_DTS := DGN1000B
  DEVICE_PACKAGES := kmod-ltq-adsl-ase kmod-ltq-adsl-ase-mei \
	kmod-ltq-adsl-ase-fw-b kmod-ltq-atm-ase \
	ltq-adsl-app ppp-mod-pppoe
  SUPPORTED_DEVICES += DGN1000B
endef
TARGET_DEVICES += netgear_dgn1000b

endif

ifeq ($(SUBTARGET),xway_legacy)

define Device/arcadyan_arv4518pwr01
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV4518PWR01
  IMAGE_SIZE := 3776k
  DEVICE_DTS := ARV4518PWR01
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-usb-ledtrig-usbport \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath5k wpad-mini
  SUPPORTED_DEVICES += ARV4518PWR01
endef
TARGET_DEVICES += arcadyan_arv4518pwr01

define Device/arcadyan_arv4518pwr01a
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV4518PWR01A
  IMAGE_SIZE := 3776k
  DEVICE_DTS := ARV4518PWR01A
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-usb-ledtrig-usbport \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath5k wpad-basic
  SUPPORTED_DEVICES += ARV4518PWR01A
endef
TARGET_DEVICES += arcadyan_arv4518pwr01a

define Device/arcadyan_arv4520pw
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV4520PW
  DEVICE_ALT0_VENDOR := Vodafone
  DEVICE_ALT0_MODEL := Easybox 800
  DEVICE_ALT1_VENDOR := Airties
  DEVICE_ALT1_MODEL := WAV-281
  IMAGE_SIZE := 3648k
  DEVICE_DTS := ARV4520PW
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-usb-ledtrig-usbport \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-rt61-pci wpad-mini
  SUPPORTED_DEVICES += ARV4520PW
endef
TARGET_DEVICES += arcadyan_arv4520pw

define Device/arcadyan_arv4525pw
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV4525PW
  DEVICE_ALT0_VENDOR := Telekom
  DEVICE_ALT0_MODEL := Speedport W502V
  DEVICE_ALT0_VARIANT := Typ A
  IMAGE_SIZE := 3776k
  DEVICE_DTS := ARV4525PW
  DEVICE_PACKAGES := kmod-ath5k wpad-mini \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa -swconfig
  SUPPORTED_DEVICES += ARV4525PW
endef
TARGET_DEVICES += arcadyan_arv4525pw

define Device/arcadyan_arv452cqw
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV452CQW
  DEVICE_ALT0_VENDOR := Vodafone
  DEVICE_ALT0_MODEL := Easybox 801
  IMAGE_SIZE := 3776k
  DEVICE_DTS := ARV452CQW
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-usb-ledtrig-usbport \
	kmod-ath5k wpad-mini \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa
  SUPPORTED_DEVICES += ARV452CQW
endef
TARGET_DEVICES += arcadyan_arv452cqw

endif

ifeq ($(SUBTARGET),xway)

# Danube

define Device/arcadyan_arv4510pw
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV4510PW
  DEVICE_ALT0_VENDOR := Wippies
  DEVICE_ALT0_MODEL := BeWan iBox v1.0
  IMAGE_SIZE := 15616k
  DEVICE_DTS := ARV4510PW
  DEVICE_PACKAGES := kmod-usb-ledtrig-usbport kmod-usb2-pci kmod-usb-uhci \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-tapi kmod-ltq-vmmc \
	kmod-rt2800-pci kmod-ath5k wpad-basic
  SUPPORTED_DEVICES += ARV4510PW
endef
TARGET_DEVICES += arcadyan_arv4510pw

define Device/arcadyan_arv4519pw
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV4519PW
  DEVICE_ALT0_VENDOR := Vodafone
  DEVICE_ALT0_MODEL := NetFasteR IAD 2
  DEVICE_ALT1_VENDOR := Pirelli
  DEVICE_ALT1_MODEL := P.RG A4201G
  IMAGE_SIZE := 3776k
  DEVICE_DTS := ARV4519PW
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-usb-ledtrig-usbport \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa
  SUPPORTED_DEVICES += ARV4519PW
endef
TARGET_DEVICES += arcadyan_arv4519pw

define Device/arcadyan_arv7506pw11
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV7506PW11
  DEVICE_ALT0_VENDOR := Alice/O2
  DEVICE_ALT0_MODEL := IAD 4421
  IMAGE_SIZE := 7808k
  DEVICE_DTS := ARV7506PW11
  DEVICE_PACKAGES := kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-rt2800-pci wpad-basic
  SUPPORTED_DEVICES += ARV7506PW11
endef
TARGET_DEVICES += arcadyan_arv7506pw11

define Device/arcadyan_arv7510pw22
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV7510PW22
  DEVICE_ALT0_VENDOR := Astoria Networks
  DEVICE_ALT0_MODEL := ARV7510PW22
  IMAGE_SIZE := 31232k
  DEVICE_DTS := ARV7510PW22
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-usb-ledtrig-usbport \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-tapi kmod-ltq-vmmc \
	kmod-rt2800-pci wpad-basic \
	kmod-usb-uhci kmod-usb2 kmod-usb2-pci
  SUPPORTED_DEVICES += ARV7510PW22
endef
TARGET_DEVICES += arcadyan_arv7510pw22

define Device/arcadyan_arv7518pw
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV7518PW
  DEVICE_ALT0_VENDOR := Astoria Networks
  DEVICE_ALT0_MODEL := ARV7518PW
  IMAGE_SIZE := 7872k
  DEVICE_DTS := ARV7518PW
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-usb-ledtrig-usbport \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath9k kmod-owl-loader wpad-basic
  SUPPORTED_DEVICES += ARV7518PW
endef
TARGET_DEVICES += arcadyan_arv7518pw

define Device/arcadyan_arv7519pw
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV7519PW
  DEVICE_ALT0_VENDOR := Astoria Networks
  DEVICE_ALT0_MODEL := ARV7519PW
  IMAGE_SIZE := 15488k
  DEVICE_DTS := ARV7519PW
  DEVICE_PACKAGES := kmod-usb-dwc2 \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-rt2800-pci wpad-basic
  SUPPORTED_DEVICES += ARV7519PW
endef
TARGET_DEVICES += arcadyan_arv7519pw

define Device/arcadyan_arv7525pw
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV7525PW
  DEVICE_ALT0_VENDOR := Telekom
  DEVICE_ALT0_MODEL := Speedport W303V
  DEVICE_ALT0_VARIANT := Typ A
  IMAGE_SIZE := 3776k
  DEVICE_DTS := ARV4510PW
  DEVICE_PACKAGES := kmod-rt2800-pci wpad-basic \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa -swconfig
  SUPPORTED_DEVICES += ARV4510PW
endef
TARGET_DEVICES += arcadyan_arv7525pw

define Device/arcadyan_arv752dpw
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV752DPW
  DEVICE_ALT0_VENDOR := Vodafone
  DEVICE_ALT0_MODEL := Easybox 802
  IMAGE_SIZE := 7872k
  DEVICE_DTS := ARV752DPW
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-usb-ledtrig-usbport \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-tapi kmod-ltq-vmmc \
	kmod-rt2800-pci wpad-basic
  SUPPORTED_DEVICES += ARV752DPW
endef
TARGET_DEVICES += arcadyan_arv752dpw

define Device/arcadyan_arv752dpw22
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV752DPW22
  DEVICE_ALT0_VENDOR := Vodafone
  DEVICE_ALT0_MODEL := Easybox 803
  IMAGE_SIZE := 7616k
  DEVICE_DTS := ARV752DPW22
  DEVICE_PACKAGES := kmod-usb2-pci kmod-usb-uhci kmod-usb-dwc2 kmod-usb-ledtrig-usbport \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-tapi kmod-ltq-vmmc \
	kmod-rt2800-pci wpad-basic
  SUPPORTED_DEVICES += ARV752DPW22
endef
TARGET_DEVICES += arcadyan_arv752dpw22

define Device/arcadyan_arv8539pw22
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV8539PW22
  DEVICE_ALT0_VENDOR := Telekom
  DEVICE_ALT0_MODEL := Speedport W504V Typ A
  IMAGE_SIZE := 7616k
  DEVICE_DTS := ARV8539PW22
  DEVICE_PACKAGES := kmod-usb-dwc2 \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath9k kmod-owl-loader wpad-basic
  SUPPORTED_DEVICES += ARV8539PW22
endef
TARGET_DEVICES += arcadyan_arv8539pw22

define Device/audiocodes_mp-252
  DEVICE_VENDOR := AudioCodes
  DEVICE_MODEL := MediaPack MP-252
  IMAGE_SIZE := 14848k
  DEVICE_DTS := ACMP252
  DEVICE_PACKAGES :=  kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	kmod-ltq-tapi kmod-ltq-vmmc \
	kmod-usb-ledtrig-usbport kmod-usb-dwc2 \
	kmod-rt2800-pci \
	ltq-adsl-app ppp-mod-pppoa \
	wpad-basic
  SUPPORTED_DEVICES += ACMP252
endef
TARGET_DEVICES += audiocodes_mp-252

define Device/bt_homehub-v2b
  $(Device/NAND)
  DEVICE_VENDOR := British Telecom
  DEVICE_MODEL := Home Hub 2
  DEVICE_VARIANT := Type B
  BOARD_NAME := BTHOMEHUBV2B
  DEVICE_DTS := BTHOMEHUBV2B
  DEVICE_PACKAGES := kmod-usb-dwc2 \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	kmod-ltq-deu-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath9k kmod-owl-loader wpad-basic
  SUPPORTED_DEVICES += BTHOMEHUBV2B
endef
TARGET_DEVICES += bt_homehub-v2b

define Device/lantiq_easy50712
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := Danube (EASY50712)
  DEVICE_DTS := EASY50712
  IMAGE_SIZE := 3776k
endef
TARGET_DEVICES += lantiq_easy50712

define Device/siemens_gigaset-sx76x
  DEVICE_VENDOR := Siemens
  DEVICE_MODEL := Gigaset sx76x
  IMAGE_SIZE := 7680k
  DEVICE_DTS := GIGASX76X
  DEVICE_PACKAGES := kmod-usb-dwc2 \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoe \
	kmod-ath5k wpad-basic
  SUPPORTED_DEVICES += GIGASX76X
endef
TARGET_DEVICES += siemens_gigaset-sx76x


# AR9

define Device/avm_fritz7312
  $(Device/AVM)
  DEVICE_MODEL := FRITZ!Box 7312
  DEVICE_DTS := FRITZ7312
  IMAGE_SIZE := 15744k
  DEVICE_PACKAGES := kmod-ath9k kmod-owl-loader wpad-basic \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-deu-ar9 -swconfig
endef
TARGET_DEVICES += avm_fritz7312

define Device/avm_fritz7320
  $(Device/AVM)
  DEVICE_MODEL := FRITZ!Box 7320
  DEVICE_ALT0_VENDOR := 1&1
  DEVICE_ALT0_MODEL := HomeServer
  DEVICE_DTS := FRITZ7320
  IMAGE_SIZE := 15744k
  DEVICE_PACKAGES := kmod-ath9k kmod-owl-loader wpad-basic \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-deu-ar9 kmod-usb-dwc2 -swconfig
  SUPPORTED_DEVICES += FRITZ7320
endef
TARGET_DEVICES += avm_fritz7320

define Device/bt_homehub-v3a
  $(Device/NAND)
  DEVICE_VENDOR := British Telecom
  DEVICE_MODEL := Home Hub
  DEVICE_VARIANT := Type A
  BOARD_NAME := BTHOMEHUBV3A
  DEVICE_DTS := BTHOMEHUBV3A
  DEVICE_PACKAGES := kmod-usb-dwc2 \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-a kmod-ltq-atm-ar9 \
	kmod-ltq-deu-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath9k kmod-owl-loader wpad-basic \
	uboot-envtools
  SUPPORTED_DEVICES += BTHOMEHUBV3A
endef
TARGET_DEVICES += bt_homehub-v3a

define Device/buffalo_wbmr-hp-g300h-a
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WBMR-HP-G300H
  DEVICE_VARIANT := A
  IMAGE_SIZE := 31488k
  DEVICE_DTS := WBMR
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-usb-ledtrig-usbport \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-a kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath9k kmod-owl-loader wpad-basic
  SUPPORTED_DEVICES := WBMR buffalo,wbmr-hp-g300h
endef
TARGET_DEVICES += buffalo_wbmr-hp-g300h-a

define Device/buffalo_wbmr-hp-g300h-b
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WBMR-HP-G300H
  DEVICE_VARIANT := B
  IMAGE_SIZE := 31488k
  DEVICE_DTS := WBMR
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-usb-ledtrig-usbport \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath9k kmod-owl-loader wpad-basic
  SUPPORTED_DEVICES := WBMR buffalo,wbmr-hp-g300h
endef
TARGET_DEVICES += buffalo_wbmr-hp-g300h-b

DGN3500_KERNEL_OFFSET_HEX=0x50000
DGN3500_KERNEL_OFFSET_DEC=327680
define Device/netgear_dgn3500
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := DGN3500
  DEVICE_DTS := DGN3500
  IMAGE_SIZE := 16000k
  IMAGES := \
	sysupgrade-na.bin sysupgrade.bin \
	factory-na.img factory.img
  IMAGE/sysupgrade-na.bin := \
	append-kernel | append-rootfs | dgn3500-sercom-footer 0x0 "NA" | \
	pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := \
	append-kernel | append-rootfs | dgn3500-sercom-footer 0x0 "WW" | \
	pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
  IMAGE/factory-na.img := \
	pad-extra $(DGN3500_KERNEL_OFFSET_DEC) | append-kernel | append-rootfs | \
	dgn3500-sercom-footer $(DGN3500_KERNEL_OFFSET_HEX) "NA" | pad-rootfs | \
	check-size 16320k | pad-to 16384k
  IMAGE/factory.img := \
	pad-extra $(DGN3500_KERNEL_OFFSET_DEC) | append-kernel | append-rootfs | \
	dgn3500-sercom-footer $(DGN3500_KERNEL_OFFSET_HEX) "WW" | pad-rootfs | \
	check-size 16320k | pad-to 16384k
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-usb-ledtrig-usbport \
	kmod-ath9k kmod-owl-loader wpad-basic \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-a kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-deu-ar9
  SUPPORTED_DEVICES += DGN3500
endef
TARGET_DEVICES += netgear_dgn3500

define Device/netgear_dgn3500b
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := DGN3500B
  DEVICE_DTS := DGN3500B
  IMAGE_SIZE := 16000k
  IMAGES += factory.img
  IMAGE/sysupgrade.bin := \
	append-kernel | append-rootfs | dgn3500-sercom-footer 0x0 "DE" | \
	pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
  IMAGE/factory.img := \
	pad-extra $(DGN3500_KERNEL_OFFSET_DEC) | append-kernel | append-rootfs | \
	dgn3500-sercom-footer $(DGN3500_KERNEL_OFFSET_HEX) "DE" | pad-rootfs | \
	check-size 16320k | pad-to 16384k
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-usb-ledtrig-usbport \
	kmod-ath9k kmod-owl-loader wpad-basic \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-deu-ar9
  SUPPORTED_DEVICES += DGN3500B
endef
TARGET_DEVICES += netgear_dgn3500b

define Device/zte_h201l
  DEVICE_VENDOR := ZTE
  DEVICE_MODEL := H201L
  IMAGE_SIZE := 7808k
  DEVICE_DTS := H201L
  DEVICE_PACKAGES := kmod-ath9k-htc wpad-basic \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoe \
	kmod-ltq-deu-ar9 kmod-usb-dwc2 kmod-usb-ledtrig-usbport \
	kmod-ltq-tapi kmod-ltq-vmmc
  SUPPORTED_DEVICES += H201L
endef
TARGET_DEVICES += zte_h201l

define Device/zyxel_p-2601hn
  DEVICE_VENDOR := ZyXEL
  DEVICE_MODEL := P-2601HN
  DEVICE_VARIANT := F1/F3
  IMAGE_SIZE := 15616k
  DEVICE_DTS := P2601HNFX
  DEVICE_PACKAGES := kmod-rt2800-usb wpad-basic \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoe \
	kmod-ltq-deu-ar9 kmod-usb-dwc2
  SUPPORTED_DEVICES += P2601HNFX
endef
TARGET_DEVICES += zyxel_p-2601hn

endif


ifeq ($(SUBTARGET),xrx200)

# VR9

include tp-link.mk

define Device/alphanetworks_asl56026
  DEVICE_VENDOR := Alpha
  DEVICE_MODEL := ASL56026
  DEVICE_ALT0_VENDOR := BT Openreach
  DEVICE_ALT0_MODEL := ECI VDSL Modem V-2FUb/I
  DEVICE_DTS := ASL56026
  IMAGE_SIZE := 7488k
endef
TARGET_DEVICES += alphanetworks_asl56026

define Device/arcadyan_arv7519rw22
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := ARV7519RW22
  DEVICE_ALT0_VENDOR := Orange
  DEVICE_ALT0_MODEL := Livebox
  DEVICE_ALT0_VARIANT := 2.1
  DEVICE_ALT1_VENDOR := Astoria Networks
  DEVICE_ALT1_MODEL := ARV7519RW22
  IMAGE_SIZE := 31232k
  DEVICE_DTS := ARV7519RW22
  DEVICE_PACKAGES := kmod-usb-dwc2
  SUPPORTED_DEVICES += ARV7519RW22
endef
TARGET_DEVICES += arcadyan_arv7519rw22

define Device/arcadyan_vg3503j
  DEVICE_VENDOR := BT Openreach
  DEVICE_MODEL := ECI VDSL Modem V-2FUb/R
  IMAGE_SIZE := 8000k
  DEVICE_DTS := VG3503J
  SUPPORTED_DEVICES += VG3503J
endef
TARGET_DEVICES += arcadyan_vg3503j

define Device/arcadyan_vgv7510kw22-brn
  $(Device/lantiqBrnImage)
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := VGV7510KW22
  DEVICE_VARIANT := BRN
  DEVICE_ALT0_VENDOR := o2
  DEVICE_ALT0_MODEL := Box 6431
  DEVICE_ALT0_VARIANT := BRN
  IMAGE_SIZE := 7168k
  DEVICE_DTS := VGV7510KW22BRN
  SIGNATURE := BRNDA6431
  MAGIC := 0x12345678
  CRC32_POLY := 0x04c11db7
  DEVICE_PACKAGES := kmod-rt2800-pci wpad-basic kmod-usb-dwc2 kmod-ltq-tapi kmod-ltq-vmmc
  SUPPORTED_DEVICES += VGV7510KW22BRN
endef
TARGET_DEVICES += arcadyan_vgv7510kw22-brn

define Device/arcadyan_vgv7510kw22-nor
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := VGV7510KW22
  DEVICE_VARIANT := NOR
  DEVICE_ALT0_VENDOR := o2
  DEVICE_ALT0_MODEL := Box 6431
  DEVICE_ALT0_VARIANT := NOR
  IMAGE_SIZE := 15232k
  DEVICE_DTS := VGV7510KW22NOR
  DEVICE_PACKAGES := kmod-rt2800-pci wpad-basic kmod-usb-dwc2 kmod-ltq-tapi kmod-ltq-vmmc
  SUPPORTED_DEVICES += VGV7510KW22NOR
endef
TARGET_DEVICES += arcadyan_vgv7510kw22-nor

define Device/arcadyan_vgv7519-brn
  $(Device/lantiqBrnImage)
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := VGV7519
  DEVICE_VARIANT := BRN
  DEVICE_ALT0_VENDOR := KPN
  DEVICE_ALT0_MODEL := Experiabox 8
  DEVICE_ALT0_VARIANT := BRN
  IMAGE_SIZE := 7168k
  DEVICE_DTS := VGV7519BRN
  SIGNATURE := 5D00008000
  MAGIC := 0x12345678
  CRC32_POLY := 0x2083b8ed
  DEVICE_PACKAGES := kmod-rt2800-pci wpad-basic kmod-usb-dwc2 kmod-ltq-tapi kmod-ltq-vmmc
  SUPPORTED_DEVICES += VGV7519BRN
endef
TARGET_DEVICES += arcadyan_vgv7519-brn

define Device/arcadyan_vgv7519-nor
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := VGV7519
  DEVICE_VARIANT := NOR
  DEVICE_ALT0_VENDOR := KPN
  DEVICE_ALT0_MODEL := Experiabox 8
  DEVICE_ALT0_VARIANT := NOR
  IMAGE_SIZE := 15360k
  DEVICE_DTS := VGV7519NOR
  DEVICE_PACKAGES := kmod-rt2800-pci wpad-basic kmod-usb-dwc2 kmod-ltq-tapi kmod-ltq-vmmc
  SUPPORTED_DEVICES += VGV7519NOR
endef
TARGET_DEVICES += arcadyan_vgv7519-nor

define Device/avm_fritz3370
  $(Device/AVM)
  $(Device/NAND)
  DEVICE_MODEL := FRITZ!Box 3370
  DEVICE_VARIANT := Rev. 2
  DEVICE_DTS := FRITZ3370
  KERNEL_SIZE := 4096k
  UBINIZE_OPTS := -E 5
  IMAGES += eva-kernel.bin eva-filesystem.bin
  IMAGE/eva-kernel.bin := append-kernel
  IMAGE/eva-filesystem.bin := append-ubi
  DEVICE_PACKAGES := kmod-ath9k wpad-basic kmod-usb-dwc2 fritz-tffs
endef

define Device/avm_fritz3370-rev2-hynix
  $(Device/avm_fritz3370)
  DEVICE_MODEL := FRITZ!Box 3370
  DEVICE_VARIANT := Rev. 2 (Hynix NAND)
  DEVICE_DTS := FRITZ3370-REV2-HYNIX
endef
TARGET_DEVICES += avm_fritz3370-rev2-hynix

define Device/avm_fritz3370-rev2-micron
  $(Device/avm_fritz3370)
  DEVICE_MODEL := FRITZ!Box 3370
  DEVICE_VARIANT := Rev. 2 (Micron NAND)
  DEVICE_DTS := FRITZ3370-REV2-MICRON
endef
TARGET_DEVICES += avm_fritz3370-rev2-micron

define Device/avm_fritz7360sl
  $(Device/AVM)
  DEVICE_MODEL := FRITZ!Box 7360 SL
  IMAGE_SIZE := 15744k
  DEVICE_DTS := FRITZ7360SL
  DEVICE_PACKAGES := kmod-ath9k kmod-owl-loader wpad-basic kmod-usb-dwc2
  SUPPORTED_DEVICES += FRITZ7360SL
endef
TARGET_DEVICES += avm_fritz7360sl

define Device/avm_fritz7362sl
  $(Device/AVM)
  $(Device/NAND)
  DEVICE_MODEL := FRITZ!Box 7362 SL
  KERNEL_SIZE := 4096k
  IMAGE_SIZE := 49152k
  DEVICE_DTS := FRITZ7362SL
  DEVICE_PACKAGES := kmod-ath9k kmod-owl-loader wpad-basic kmod-usb-dwc2 fritz-tffs
endef
TARGET_DEVICES += avm_fritz7362sl

define Device/avm_fritz7412
  $(Device/AVM)
  $(Device/NAND)
  DEVICE_MODEL := FRITZ!Box 7412
  BOARD_NAME := FRITZ7412
  DEVICE_DTS := FRITZ7412
  KERNEL_SIZE := 4096k
  IMAGE_SIZE := 49152k
  DEVICE_PACKAGES := kmod-ath9k kmod-owl-loader wpad-basic fritz-tffs-nand fritz-caldata
endef
TARGET_DEVICES += avm_fritz7412

define Device/bt_homehub-v5a
  $(Device/NAND)
  DEVICE_VENDOR := British Telecom
  DEVICE_MODEL := Home Hub 5
  DEVICE_VARIANT := Type A
  BOARD_NAME := BTHOMEHUBV5A
  DEVICE_DTS := BTHOMEHUBV5A
  DEVICE_PACKAGES := kmod-ath9k kmod-owl-loader \
	kmod-ath10k-ct ath10k-firmware-qca988x-ct wpad-basic kmod-usb-dwc2
  SUPPORTED_DEVICES += BTHOMEHUBV5A
endef
TARGET_DEVICES += bt_homehub-v5a

define Device/buffalo_wbmr-300hpd
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WBMR-300HPD
  IMAGE_SIZE := 15616k
  DEVICE_DTS := WBMR300
  DEVICE_PACKAGES := kmod-mt7603 wpad-basic kmod-usb-dwc2
  SUPPORTED_DEVICES += WBMR300
endef
TARGET_DEVICES += buffalo_wbmr-300hpd

define Device/lantiq_easy80920-nand
  $(Device/lantiqFullImage)
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := VR9 EASY80920
  DEVICE_VARIANT := NAND
  DEVICE_DTS := EASY80920NAND
  IMAGE_SIZE := 64512k
  DEVICE_PACKAGES := kmod-ath9k kmod-owl-loader wpad-basic kmod-usb-dwc2 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += lantiq_easy80920-nand

define Device/lantiq_easy80920-nor
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := VR9 EASY80920
  DEVICE_VARIANT := NOR
  DEVICE_DTS := EASY80920NOR
  IMAGE_SIZE := 7936k
  DEVICE_PACKAGES := kmod-ath9k kmod-owl-loader wpad-basic kmod-usb-dwc2 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += lantiq_easy80920-nor

define Device/netgear_dm200
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := DM200
  DEVICE_DTS := DM200
  IMAGES := sysupgrade.bin factory.img
  IMAGE/sysupgrade.bin := append-kernel | \
	pad-offset 64k 64 | append-uImage-fakehdr filesystem | \
	pad-offset 64k 64 | append-uImage-fakehdr filesystem | \
	append-rootfs | pad-rootfs | append-metadata | check-size $$$$(IMAGE_SIZE)
  IMAGE/factory.img := $$(IMAGE/sysupgrade.bin) | netgear-dni
  IMAGE_SIZE := 7872k
  NETGEAR_BOARD_ID := DM200
  NETGEAR_HW_ID := 29765233+8+0+64+0+0
endef
DEVICE_VARS += NETGEAR_BOARD_ID NETGEAR_HW_ID
TARGET_DEVICES += netgear_dm200

define Device/zyxel_p-2812hnu-f1
  $(Device/NAND)
  DEVICE_VENDOR := ZyXEL
  DEVICE_MODEL := P-2812HNU
  DEVICE_VARIANT := F1
  BOARD_NAME := P2812HNUF1
  DEVICE_DTS := P2812HNUF1
  DEVICE_PACKAGES := kmod-rt2800-pci wpad-basic kmod-usb-dwc2 kmod-usb-ledtrig-usbport
  KERNEL_SIZE := 3072k
  SUPPORTED_DEVICES += P2812HNUF1
endef
TARGET_DEVICES += zyxel_p-2812hnu-f1

define Device/zyxel_p-2812hnu-f3
  $(Device/NAND)
  DEVICE_VENDOR := ZyXEL
  DEVICE_MODEL := P-2812HNU
  DEVICE_VARIANT := F3
  BOARD_NAME := P2812HNUF3
  DEVICE_DTS := P2812HNUF3
  DEVICE_PACKAGES := kmod-rt2800-pci wpad-basic kmod-usb-dwc2
  SUPPORTED_DEVICES += P2812HNUF3
endef
TARGET_DEVICES += zyxel_p-2812hnu-f3

endif


ifeq ($(SUBTARGET),falcon)

define Device/lantiq_easy88388
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := EASY88388 Falcon FTTDP8 Reference Board
  IMAGE_SIZE := 7424k
  DEVICE_DTS := EASY88388
endef
TARGET_DEVICES += lantiq_easy88388

define Device/lantiq_easy88444
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := EASY88444 Falcon FTTdp G.FAST Reference Board
  IMAGE_SIZE := 7424k
  DEVICE_DTS := EASY88444
endef
TARGET_DEVICES += lantiq_easy88444

define Device/lantiq_easy98020
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := Falcon SFU Reference Board (EASY98020)
  DEVICE_VARIANT := v1.0-v1.7
  IMAGE_SIZE := 7424k
  DEVICE_DTS := EASY98020
endef
TARGET_DEVICES += lantiq_easy98020

define Device/lantiq_easy98020-v18
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := Falcon SFU Reference Board (EASY98020)
  DEVICE_VARIANT := v1.8
  IMAGE_SIZE := 7424k
  DEVICE_DTS := EASY98020V18
endef
TARGET_DEVICES += lantiq_easy98020-v18

define Device/lantiq_easy98021
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := Falcon HGU Reference Board (EASY98021)
  IMAGE_SIZE := 7424k
  DEVICE_DTS := EASY98021
endef
TARGET_DEVICES += lantiq_easy98021

define Device/lantiq_easy98035synce
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := Falcon SFP Stick (EASY98035SYNCE)
  DEVICE_VARIANT := with Synchronous Ethernet
  IMAGE_SIZE := 7424k
  DEVICE_DTS := EASY98035SYNCE
endef
TARGET_DEVICES += lantiq_easy98035synce

define Device/lantiq_easy98035synce1588
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := Falcon SFP Stick (EASY98035SYNCE1588)
  DEVICE_VARIANT := with SyncE and IEEE1588
  IMAGE_SIZE := 7424k
  DEVICE_DTS := EASY98035SYNCE1588
endef
TARGET_DEVICES += lantiq_easy98035synce1588

define Device/lantiq_easy98000-nand
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := EASY98000 Falcon Eval Board
  DEVICE_VARIANT := NAND
  IMAGE_SIZE := 3904k
  DEVICE_DTS := EASY98000NAND
  DEVICE_PACKAGES := kmod-dm9000 kmod-i2c-lantiq kmod-eeprom-at24
endef
TARGET_DEVICES += lantiq_easy98000-nand

define Device/lantiq_easy98000-nor
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := EASY98000 Falcon Eval Board
  DEVICE_VARIANT := NOR
  IMAGE_SIZE := 3904k
  DEVICE_DTS := EASY98000NOR
  DEVICE_PACKAGES := kmod-dm9000 kmod-i2c-lantiq kmod-eeprom-at24
endef
TARGET_DEVICES += lantiq_easy98000-nor

define Device/lantiq_easy98000-sflash
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := EASY98000 Falcon Eval Board
  DEVICE_VARIANT := SFLASH
  IMAGE_SIZE := 7424k
  DEVICE_DTS := EASY98000SFLASH
  DEVICE_PACKAGES := kmod-dm9000 kmod-i2c-lantiq kmod-eeprom-at24
endef
TARGET_DEVICES += lantiq_easy98000-sflash

define Device/lantiq_falcon-mdu
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := Falcon / VINAXdp MDU Board
  IMAGE_SIZE := 7424k
  DEVICE_DTS := FALCON-MDU
endef
TARGET_DEVICES += lantiq_falcon-mdu

define Device/lantiq_falcon-sfp
  DEVICE_VENDOR := Lantiq
  DEVICE_MODEL := Falcon SFP Stick
  IMAGE_SIZE := 7424k
  DEVICE_DTS := FALCON-SFP
endef
TARGET_DEVICES += lantiq_falcon-sfp

endif

$(eval $(call BuildImage))
