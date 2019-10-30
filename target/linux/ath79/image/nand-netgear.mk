include ./common-netgear.mk	# for netgear-uImage

# (pad-offset 129 = 2 * uimage_header + 0xff)
define Device/netgear_ath79_nand
  DEVICE_VENDOR := NETGEAR
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ledtrig-usbport
  KERNEL_SIZE := 2048k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 25600k
  KERNEL := kernel-bin | append-dtb | lzma -d20 | \
	pad-offset $$(KERNEL_SIZE) 129 | \
	netgear-uImage lzma | append-string -e '\xff' | \
	append-uImage-fakehdr filesystem $$(NETGEAR_KERNEL_MAGIC)
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma -d20 | \
	netgear-uImage lzma
  IMAGES := ubi-sysupgrade.bin ubi-factory.img
  IMAGE/ubi-factory.img := append-kernel | append-ubi | netgear-dni | \
	check-size $$$$(IMAGE_SIZE)
  IMAGE/ubi-sysupgrade.bin := sysupgrade-tar | append-metadata | \
	check-size $$$$(IMAGE_SIZE)
  UBINIZE_OPTS := -E 5
endef

define Device/netgear_wndr4300
  ATH_SOC := ar9344
  DEVICE_MODEL := WNDR4300
  NETGEAR_KERNEL_MAGIC := 0x33373033
  NETGEAR_BOARD_ID := WNDR4300
  NETGEAR_HW_ID := 29763948+0+128+128+2x2+3x3
  SUPPORTED_DEVICES += wndr4300
  $(Device/netgear_ath79_nand)
endef
TARGET_DEVICES += netgear_wndr4300
