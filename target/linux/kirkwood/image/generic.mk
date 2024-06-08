define Device/checkpoint_l-50
  DEVICE_VENDOR := Check Point
  DEVICE_MODEL := L-50
  DEVICE_PACKAGES := kmod-ath9k kmod-gpio-button-hotplug kmod-mvsdio \
	kmod-rtc-s35390a kmod-usb-ledtrig-usbport wpad-basic-mbedtls
  IMAGES := sysupgrade.bin
endef
TARGET_DEVICES += checkpoint_l-50

define Device/cisco_on100
  DEVICE_VENDOR := Cisco Systems
  DEVICE_MODEL := ON100
  KERNEL_SIZE := 5376k
  KERNEL_IN_UBI :=
  UBINIZE_OPTS := -E 5
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi
  DEVICE_PACKAGES := kmod-mvsdio kmod-rtc-mv
  SUPPORTED_DEVICES += on100
endef
TARGET_DEVICES += cisco_on100

define Device/cloudengines_pogoe02
  DEVICE_VENDOR := Cloud Engines
  DEVICE_MODEL := Pogoplug E02
  DEVICE_DTS := kirkwood-pogo_e02
  DEVICE_PACKAGES := kmod-rtc-mv
  SUPPORTED_DEVICES += pogo_e02
endef
TARGET_DEVICES += cloudengines_pogoe02

define Device/cloudengines_pogoplugv4
  DEVICE_VENDOR := Cloud Engines
  DEVICE_MODEL := Pogoplug V4
  DEVICE_DTS := kirkwood-pogoplug-series-4
  DEVICE_PACKAGES := kmod-ata-marvell-sata kmod-fs-ext4 kmod-mvsdio kmod-usb3 \
	kmod-gpio-button-hotplug kmod-rtc-mv
endef
TARGET_DEVICES += cloudengines_pogoplugv4

define Device/ctera_c200-v1
  DEVICE_VENDOR := Ctera
  DEVICE_MODEL := C200
  DEVICE_VARIANT := V1
  DEVICE_PACKAGES := kmod-ata-marvell-sata kmod-gpio-button-hotplug \
	kmod-hwmon-lm63 kmod-rtc-s35390a kmod-usb-ledtrig-usbport
  KERNEL := kernel-bin | append-dtb | uImage none | ctera-firmware
  KERNEL_IN_UBI :=
  KERNEL_SUFFIX := -factory.firm
  IMAGES := sysupgrade.bin
endef
TARGET_DEVICES += ctera_c200-v1

define Device/endian_4i-edge-200
  DEVICE_VENDOR := Endian
  DEVICE_MODEL := 4i Edge 200
  DEVICE_ALT0_VENDOR := Endian
  DEVICE_ALT0_MODEL := UTM Mini Firewall
  DEVICE_PACKAGES := kmod-ath9k kmod-mvsdio wpad-basic-mbedtls kmod-rtc-mv
  KERNEL_SIZE := 4096k
  IMAGES := sysupgrade.bin
endef
TARGET_DEVICES += endian_4i-edge-200

define Device/globalscale_sheevaplug
  DEVICE_VENDOR := Globalscale
  DEVICE_MODEL := Sheevaplug
  DEVICE_PACKAGES := kmod-mvsdio kmod-rtc-mv
endef
TARGET_DEVICES += globalscale_sheevaplug

define Device/iom_iconnect-1.1
  DEVICE_VENDOR := Iomega
  DEVICE_MODEL := Iconnect
  DEVICE_PACKAGES := kmod-rtc-mv
  DEVICE_DTS := kirkwood-iconnect
  SUPPORTED_DEVICES += iconnect
endef
TARGET_DEVICES += iom_iconnect-1.1

define Device/iptime_nas1
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := NAS1
  DEVICE_PACKAGES := kmod-ata-marvell-sata kmod-fs-ext4 \
	kmod-gpio-button-hotplug kmod-gpio-pca953x kmod-hwmon-drivetemp \
	kmod-hwmon-gpiofan kmod-usb-ledtrig-usbport kmod-rtc-mv kmod-thermal \
  -uboot-envtools
  KERNEL := $$(KERNEL) | iptime-naspkg nas1
  BLOCKSIZE := 256k
  IMAGE_SIZE := 15872k
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | \
	check-size | append-metadata
endef
TARGET_DEVICES += iptime_nas1

define Device/netgear_readynas-duo-v2
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := ReadyNAS Duo
  DEVICE_VARIANT := v2
  DEVICE_DTS := kirkwood-netgear_readynas_duo_v2
  KERNEL_IN_UBI :=
  IMAGES := sysupgrade.bin
  DEVICE_PACKAGES := kmod-ata-marvell-sata kmod-fs-ext4 \
	kmod-gpio-button-hotplug kmod-hwmon-g762 kmod-rtc-rs5c372a kmod-usb3
endef
TARGET_DEVICES += netgear_readynas-duo-v2

define Device/raidsonic_ib-nas62x0
  DEVICE_VENDOR := RaidSonic
  DEVICE_MODEL := ICY BOX IB-NAS62x0
  DEVICE_DTS := kirkwood-ib62x0
  DEVICE_PACKAGES := kmod-ata-marvell-sata kmod-fs-ext4 kmod-rtc-mv
  SUPPORTED_DEVICES += ib62x0
endef
TARGET_DEVICES += raidsonic_ib-nas62x0

define Device/seagate_blackarmor-nas220
  DEVICE_VENDOR := Seagate
  DEVICE_MODEL := Blackarmor NAS220
  DEVICE_PACKAGES := kmod-hwmon-adt7475 kmod-fs-ext4 kmod-ata-marvell-sata \
	mdadm kmod-gpio-button-hotplug kmod-rtc-mv
  PAGESIZE := 512
  SUBPAGESIZE := 256
  BLOCKSIZE := 16k
  UBINIZE_OPTS := -e 1
endef
TARGET_DEVICES += seagate_blackarmor-nas220

define Device/seagate_dockstar
  DEVICE_VENDOR := Seagate
  DEVICE_MODEL := FreeAgent Dockstar
  DEVICE_PACKAGES := kmod-rtc-mv
  SUPPORTED_DEVICES += dockstar
endef
TARGET_DEVICES += seagate_dockstar

define Device/seagate_goflexnet
  DEVICE_VENDOR := Seagate
  DEVICE_MODEL := GoFlexNet
  DEVICE_PACKAGES := kmod-ata-marvell-sata kmod-fs-ext4 kmod-rtc-mv
  SUPPORTED_DEVICES += goflexnet
endef
TARGET_DEVICES += seagate_goflexnet

define Device/seagate_goflexhome
  DEVICE_VENDOR := Seagate
  DEVICE_MODEL := GoFlexHome
  DEVICE_PACKAGES := kmod-ata-marvell-sata kmod-fs-ext4 kmod-rtc-mv
  SUPPORTED_DEVICES += goflexhome
endef
TARGET_DEVICES += seagate_goflexhome

define Device/zyxel_nsa310b
  DEVICE_VENDOR := ZyXEL
  DEVICE_MODEL := NSA310b
  DEVICE_PACKAGES := kmod-ata-marvell-sata kmod-r8169 kmod-fs-ext4 \
	kmod-gpio-button-hotplug kmod-hwmon-lm85 kmod-rtc-mv
  SUPPORTED_DEVICES += nsa310b
endef
TARGET_DEVICES += zyxel_nsa310b

define Device/zyxel_nsa310s
  DEVICE_VENDOR := ZyXEL
  DEVICE_MODEL := NSA310S
  DEVICE_PACKAGES := kmod-ata-marvell-sata kmod-fs-ext4 kmod-gpio-button-hotplug
endef
TARGET_DEVICES += zyxel_nsa310s

define Device/zyxel_nsa325
  DEVICE_VENDOR := ZyXEL
  DEVICE_MODEL := NSA325
  DEVICE_VARIANT := v1/v2
  DEVICE_PACKAGES := kmod-ata-marvell-sata kmod-fs-ext4 \
	kmod-gpio-button-hotplug kmod-rtc-pcf8563 kmod-usb3
  SUPPORTED_DEVICES += nsa325
endef
TARGET_DEVICES += zyxel_nsa325
