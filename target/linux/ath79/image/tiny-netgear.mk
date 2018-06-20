include ./common-netgear.mk


define Device/netgear_wnr612-v2
  ATH_SOC := ar7240
  DEVICE_TITLE := Netgear WNR612v2
  DEVICE_DTS := ar7240_netgear_wnr612-v2
  NETGEAR_KERNEL_MAGIC := 0x32303631
  KERNEL := kernel-bin | append-dtb | lzma -d20 | netgear-uImage lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma -d20 | netgear-uImage lzma
  NETGEAR_BOARD_ID := REALWNR612V2
  IMAGE_SIZE := 3904k
  IMAGES := sysupgrade.bin factory.img
  IMAGE/default := append-kernel | pad-to $$$$(BLOCKSIZE) | netgear-squashfs | append-rootfs | pad-rootfs
  IMAGE/sysupgrade.bin := $$(IMAGE/default) | append-metadata | check-size $$$$(IMAGE_SIZE)
  IMAGE/factory.img := $$(IMAGE/default) | netgear-dni | check-size $$$$(IMAGE_SIZE)
  SUPPORTED_DEVICES := netgear,wnr612-v2 wnr612-v2
endef
TARGET_DEVICES += netgear_wnr612-v2
