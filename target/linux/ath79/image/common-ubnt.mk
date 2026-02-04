DEVICE_VARS += UBNT_BOARD UBNT_CHIP UBNT_TYPE UBNT_VERSION UBNT_REVISION

# On M (XW) devices the U-Boot as of version 1.1.4-s1039 doesn't like
# VERSION_DIST being on the place of major(?) version number, so we need to
# use some number.
UBNT_REVISION := $(VERSION_DIST)-$(REVISION)

# mkubntimage is using the kernel image direct
# routerboard creates partitions out of the ubnt header
define Build/mkubntimage
	-$(STAGING_DIR_HOST)/bin/mkfwimage -B $(UBNT_BOARD) \
		-v $(UBNT_TYPE).$(UBNT_CHIP).v6.0.0-$(VERSION_DIST)-$(REVISION) \
		-k $(if $(1),$(1),$(IMAGE_KERNEL)) -r $@ -o $@
endef

define Build/mkubntimage2
	-$(STAGING_DIR_HOST)/bin/mkfwimage2 -f 0x9f000000 \
		-v $(UBNT_TYPE).$(UBNT_CHIP).v6.0.0-$(VERSION_DIST)-$(REVISION) \
		-p jffs2:0x50000:0xf60000:0:0:$@ \
		-o $@.new
	@mv $@.new $@
endef

# all UBNT XM/WA devices expect the kernel image to have 1024k while flash, when
# booting the image, the size doesn't matter.
define Build/mkubntimage-split
	-[ -f $@ ] && ( \
	dd if=$@ of=$@.old1 bs=1024k count=1; \
	dd if=$@ of=$@.old2 bs=1024k skip=1; \
	$(STAGING_DIR_HOST)/bin/mkfwimage -B $(UBNT_BOARD) \
		-v $(UBNT_TYPE).$(UBNT_CHIP).v$(UBNT_VERSION)-$(UBNT_REVISION) \
		-k $@.old1 -r $@.old2 -o $@; \
	rm $@.old1 $@.old2 )
endef

# UBNT_BOARD e.g. one of (XS2, XS5, RS, XM)
# UBNT_TYPE e.g. one of (BZ, XM, XW)
# UBNT_CHIP e.g. one of (ar7240, ar933x, ar934x)
# UBNT_VERSION e.g. one of (6.0.0, 8.5.3)
define Device/ubnt
  DEVICE_VENDOR := Ubiquiti
  DEVICE_PACKAGES := kmod-usb2
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$$$(BLOCKSIZE) | \
	append-rootfs | pad-rootfs | check-size | mkubntimage-split
endef

define Device/ubnt-bz
  $(Device/ubnt)
  SOC := ar7241
  IMAGE_SIZE := 7448k
  UBNT_BOARD := XM
  UBNT_CHIP := ar7240
  UBNT_TYPE := BZ
  UBNT_VERSION := 6.0.0
endef

define Device/ubnt-sw
  $(Device/ubnt)
  SOC := ar7242
  DEVICE_PACKAGES += kmod-usb-ohci
  IMAGE_SIZE := 7552k
  UBNT_BOARD := SW
  UBNT_CHIP := ar7240
  UBNT_TYPE := SW
  UBNT_VERSION := 1.4.1
  KERNEL := kernel-bin | append-dtb | relocate-kernel | lzma | uImage lzma
endef

define Device/ubnt-2wa
  $(Device/ubnt)
  SOC := ar9342
  IMAGE_SIZE := 15744k
  UBNT_BOARD := WA
  UBNT_CHIP := ar934x
  UBNT_TYPE := 2WA
  UBNT_VERSION := 8.7.4
endef

define Device/ubnt-wa
  $(Device/ubnt)
  SOC := ar9342
  IMAGE_SIZE := 15744k
  UBNT_BOARD := WA
  UBNT_CHIP := ar934x
  UBNT_TYPE := WA
  UBNT_VERSION := 8.7.4
endef

define Device/ubnt-xc
  $(Device/ubnt)
  IMAGE_SIZE := 15744k
  UBNT_BOARD := XC
  UBNT_CHIP := qca955x
  UBNT_TYPE := XC
  UBNT_VERSION := 8.7.4
endef

define Device/ubnt-xm
  $(Device/ubnt)
  DEVICE_VARIANT := XM
  DEVICE_PACKAGES += kmod-usb-ohci
  IMAGE_SIZE := 7448k
  UBNT_BOARD := XM
  UBNT_CHIP := ar7240
  UBNT_REVISION := 42.$(UBNT_REVISION)
  UBNT_TYPE := XM
  UBNT_VERSION := 6.0.0
  KERNEL := kernel-bin | append-dtb | relocate-kernel | lzma | uImage lzma
endef

define Device/ubnt-xw
  $(Device/ubnt)
  SOC := ar9342
  DEVICE_VARIANT := XW
  IMAGE_SIZE := 7552k
  UBNT_BOARD := XM
  UBNT_CHIP := ar934x
  UBNT_REVISION := 42.$(UBNT_REVISION)
  UBNT_TYPE := XW
  UBNT_VERSION := 6.0.4
endef

define Device/ubnt_unifiac
  DEVICE_VENDOR := Ubiquiti
  SOC := qca9563
  IMAGE_SIZE := 15488k
  DEVICE_PACKAGES := kmod-ath10k-ct ath10k-firmware-qca988x-ct
endef

define Device/ubnt-unifi-jffs2
  $(Device/ubnt)
  KERNEL_SIZE := 3072k
  IMAGE_SIZE := 15744k
  UBNT_TYPE := BZ
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma | jffs2 kernel0
  IMAGES := sysupgrade.bin factory.bin
  IMAGE/sysupgrade.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-rootfs |\
	pad-rootfs | check-size | append-metadata
  IMAGE/factory.bin := $$(IMAGE/sysupgrade.bin) | mkubntimage2
endef

define Device/ubnt-acb
  $(Device/ubnt)
  IMAGE_SIZE := 15744k
  UBNT_BOARD := ACB
  UBNT_TYPE := ACB
  UBNT_VERSION := 2.5.0
endef
