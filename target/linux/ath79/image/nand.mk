define Device/glinet_gl-ar150
  ATH_SOC := ar9330
  DEVICE_TITLE := GL.iNet GL-AR150
  DEVICE_PACKAGES := kmod-usb2  block-mount
  IMAGE_SIZE := 16000k
  SUPPORTED_DEVICES += gl-ar150 glinet,gl-ar150
endef
TARGET_DEVICES += glinet_gl-ar150

define Device/glinet_gl-usb150
  ATH_SOC := ar9330
  DEVICE_TITLE := GL.iNet GL-USB150
  DEVICE_PACKAGES := kmod-usb2  block-mount
  IMAGE_SIZE := 16000k
  SUPPORTED_DEVICES += gl-usb150 glinet,gl-usb150
endef
TARGET_DEVICES += glinet_gl-usb150

define Device/glinet_gl-mifi
  ATH_SOC := ar9330
  DEVICE_TITLE := GL.iNet GL-MIFI
  IMAGE_SIZE := 16000k
  SUPPORTED_DEVICES += gl-mifi glinet,gl-mifi
endef
TARGET_DEVICES += glinet_gl-mifi

define Device/glinet_gl-s200-nor
  ATH_SOC := qca9531
  DEVICE_TITLE := GL.iNet GL-S200 (NOR)
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-serial-ch341
  IMAGE_SIZE := 16000k
  SUPPORTED_DEVICES += gl-s200 glinet,gl-s200
endef
TARGET_DEVICES += glinet_gl-s200-nor

define Device/glinet_gl-s200-nor-nand
  ATH_SOC := qca9531
  DEVICE_TITLE := GL.iNet GL-S200 (NOR/NAND)
  DEVICE_PACKAGES := kmod-usb2 block-mount kmod-usb-serial-ch341
  KERNEL_SIZE := 4096k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  VID_HDR_OFFSET := 2048
  IMAGES := factory.img sysupgrade.tar
  IMAGE/sysupgrade.tar := sysupgrade-tar-compat-1806 | append-gl-metadata
  IMAGE/factory.img := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | append-gl-metadata
  SUPPORTED_DEVICES += gl-s200 glinet,gl-s200
endef
TARGET_DEVICES += glinet_gl-s200-nor-nand

define Device/glinet_gl-ar300m-nor
  ATH_SOC := qca9531
  DEVICE_TITLE := GL.iNet GL-AR300M (NOR)
  DEVICE_PACKAGES := kmod-usb2  block-mount
  IMAGE_SIZE := 16000k
  SUPPORTED_DEVICES += gl-ar300m glinet,gl-ar300m
endef
TARGET_DEVICES += glinet_gl-ar300m-nor

define Device/glinet_gl-ar300m-nand
  ATH_SOC := qca9531
  DEVICE_TITLE := GL-AR300M (NAND)
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage kmod-usb-ledtrig-usbport
#  KERNEL_SIZE := 2048k
  KERNEL_SIZE := 4096k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  VID_HDR_OFFSET := 2048
  IMAGES := factory.img sysupgrade.tar
  IMAGE/sysupgrade.tar := sysupgrade-tar-compat-1806 | append-gl-metadata
  IMAGE/factory.img := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | append-gl-metadata
  SUPPORTED_DEVICES += gl-ar300m glinet,gl-ar300m
endef
TARGET_DEVICES += glinet_gl-ar300m-nand

define Device/glinet_gl-ar750
  ATH_SOC := qca9531
  DEVICE_TITLE := GL.iNet GL-AR750
  DEVICE_PACKAGES := kmod-usb2 kmod-ath10k ath10k-firmware-qca9887 block-mount PCI_SUPPORT
  IMAGE_SIZE := 16000k
  SUPPORTED_DEVICES += gl-ar750 glinet,gl-ar750
endef
TARGET_DEVICES += glinet_gl-ar750

define Device/glinet_gl-ar750s-nor
  ATH_SOC := qca9563
  DEVICE_TITLE := GL.iNet GL-AR750S (NOR)
  DEVICE_PACKAGES := kmod-usb2 kmod-ath10k ath10k-firmware-qca9887 block-mount
  IMAGE_SIZE := 16000k
  SUPPORTED_DEVICES += gl-ar750s glinet,gl-ar750s
endef
TARGET_DEVICES += glinet_gl-ar750s-nor

define Device/glinet_gl-ar750s-nor-nand
  ATH_SOC := qca9563
  DEVICE_TITLE := GL.iNet GL-AR750S (NOR/NAND)
  DEVICE_PACKAGES := kmod-usb2 kmod-ath10k ath10k-firmware-qca9887 block-mount PCI_SUPPORT
#  KERNEL_SIZE := 2048k
  KERNEL_SIZE := 4096k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  VID_HDR_OFFSET := 2048
  IMAGES := factory.img sysupgrade.tar
  IMAGE/sysupgrade.tar := sysupgrade-tar | append-gl-metadata
  IMAGE/factory.img := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | append-gl-metadata
  SUPPORTED_DEVICES += gl-ar750s glinet,gl-ar750s
endef
TARGET_DEVICES += glinet_gl-ar750s-nor-nand

define Device/glinet_gl-e750-nor
  ATH_SOC := qca9531
  DEVICE_TITLE := GL.iNet GL-E750 (NOR)
  DEVICE_PACKAGES := kmod-usb2 kmod-ath10k ath10k-firmware-qca9887 block-mount
  IMAGE_SIZE := 16000k
  SUPPORTED_DEVICES += gl-e750 glinet,gl-e750
endef
TARGET_DEVICES += glinet_gl-e750-nor

define Device/glinet_gl-e750-nor-nand
  ATH_SOC := qca9531
  DEVICE_TITLE := GL.iNet GL-E750 (NOR/NAND)
  DEVICE_PACKAGES := kmod-usb2 kmod-ath10k ath10k-firmware-qca9887 block-mount PCI_SUPPORT
#  KERNEL_SIZE := 2048k
  KERNEL_SIZE := 4096k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  VID_HDR_OFFSET := 2048
  IMAGES := factory.img sysupgrade.tar
  IMAGE/sysupgrade.tar := sysupgrade-tar-compat-1806 | append-gl-metadata
  IMAGE/factory.img := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | append-gl-metadata
  SUPPORTED_DEVICES += gl-e750 glinet,gl-e750
endef
TARGET_DEVICES += glinet_gl-e750-nor-nand

define Device/glinet_gl-x750-nor
  ATH_SOC := qca9531
  DEVICE_TITLE := GL.iNet GL-X750 (NOR)
  DEVICE_PACKAGES := kmod-usb2 kmod-ath10k ath10k-firmware-qca9887 block-mount
  IMAGE_SIZE := 16000k
  SUPPORTED_DEVICES += gl-x750 glinet,gl-x750
endef
TARGET_DEVICES += glinet_gl-x750-nor

define Device/glinet_gl-x750-nor-nand
  ATH_SOC := qca9531
  DEVICE_TITLE := GL.iNet GL-X750 (NOR/NAND)
  DEVICE_PACKAGES := kmod-usb2 kmod-ath10k ath10k-firmware-qca9887 block-mount PCI_SUPPORT
#  KERNEL_SIZE := 2048k
  KERNEL_SIZE := 4096k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  VID_HDR_OFFSET := 2048
  IMAGES := factory.img sysupgrade.tar
  IMAGE/sysupgrade.tar := sysupgrade-tar | append-gl-metadata
  IMAGE/factory.img := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | append-gl-metadata
  SUPPORTED_DEVICES += gl-x750 glinet,gl-x750
endef
TARGET_DEVICES += glinet_gl-x750-nor-nand

define Device/glinet_gl-xe300-nor
  ATH_SOC := qca9531
  DEVICE_TITLE := GL.iNet GL-XE300 (NOR)
  DEVICE_PACKAGES := kmod-usb2 block-mount  kmod-usb-serial-ch341
  IMAGE_SIZE := 16000k
  SUPPORTED_DEVICES += gl-xe300 glinet,gl-xe300
endef
TARGET_DEVICES += glinet_gl-xe300-nor

define Device/glinet_gl-xe300-nor-nand
  ATH_SOC := qca9531
  DEVICE_TITLE := GL.iNet GL-XE300 (NOR/NAND)
  DEVICE_PACKAGES := kmod-usb2 block-mount kmod-usb-serial-ch341
#  KERNEL_SIZE := 2048k
  KERNEL_SIZE := 4096k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  VID_HDR_OFFSET := 2048
  IMAGES := factory.img sysupgrade.tar
  IMAGE/sysupgrade.tar := sysupgrade-tar-compat-1806 | append-gl-metadata
  IMAGE/factory.img := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | append-gl-metadata
  SUPPORTED_DEVICES += gl-xe300 glinet,gl-xe300
endef
TARGET_DEVICES += glinet_gl-xe300-nor-nand

define Device/glinet_gl-xe300-iot
  ATH_SOC := qca9531
  DEVICE_TITLE := GL.iNet GL-XE300 (NOR/NAND IOT)
  DEVICE_PACKAGES := kmod-usb2 block-mount kmod-usb-serial-ch341
  KERNEL_SIZE := 2048k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  VID_HDR_OFFSET := 2048
  IMAGES := factory.img sysupgrade.tar
  IMAGE/sysupgrade.tar := sysupgrade-tar-compat-1806 | append-gl-metadata
  IMAGE/factory.img := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | append-gl-metadata
  SUPPORTED_DEVICES += gl-xe300 glinet,gl-xe300
endef
TARGET_DEVICES += glinet_gl-xe300-iot

define Device/glinet_gl-x300b-nor
  ATH_SOC := qca9531
  DEVICE_TITLE := GL.iNet GL-X300B (NOR)
  DEVICE_PACKAGES := kmod-usb2 block-mount
  IMAGE_SIZE := 16000k
  SUPPORTED_DEVICES += gl-x300b glinet,gl-x300b
endef
TARGET_DEVICES += glinet_gl-x300b-nor

define Device/glinet_gl-x300b-nor-nand
  ATH_SOC := qca9531
  DEVICE_TITLE := GL.iNet GL-X300B (NOR/NAND)
  DEVICE_PACKAGES := kmod-usb2 block-mount
#  KERNEL_SIZE := 2048k
  KERNEL_SIZE := 4096k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  VID_HDR_OFFSET := 2048
  IMAGES := factory.img sysupgrade.tar
  IMAGE/sysupgrade.tar := sysupgrade-tar-compat-1806 | append-gl-metadata
  IMAGE/factory.img := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | append-gl-metadata
  SUPPORTED_DEVICES += gl-x300b glinet,gl-x300b
endef
TARGET_DEVICES += glinet_gl-x300b-nor-nand

define Device/glinet_gl-x1200-nor
  ATH_SOC := qca9563
  DEVICE_TITLE := GL.iNet GL-X1200 (NOR)
  DEVICE_PACKAGES := kmod-usb2 kmod-ath10k-ct ath10k-firmware-qca9887-ct-htt block-mount
  IMAGE_SIZE := 16000k
  SUPPORTED_DEVICES += gl-x1200 glinet,gl-x1200
endef
TARGET_DEVICES += glinet_gl-x1200-nor

define Device/glinet_gl-x1200-nor-nand
  ATH_SOC := qca9563
  DEVICE_TITLE := GL.iNet GL-X1200 (NOR/NAND)
  DEVICE_PACKAGES := kmod-usb2 kmod-ath10k-ct ath10k-firmware-qca9887-ct-htt block-mount PCI_SUPPORT
#  KERNEL_SIZE := 2048k
  KERNEL_SIZE := 4096k
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  VID_HDR_OFFSET := 2048
  IMAGES := factory.img sysupgrade.tar
  IMAGE/sysupgrade.tar := sysupgrade-tar-compat-1806 | append-gl-metadata
  IMAGE/factory.img := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | append-gl-metadata
  SUPPORTED_DEVICES += gl-x1200 glinet,gl-x1200
endef
TARGET_DEVICES += glinet_gl-x1200-nor-nand
