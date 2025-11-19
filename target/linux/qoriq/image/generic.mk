define Device/fsl_t4240rdb
  DEVICE_VENDOR := NXP
  DEVICE_MODEL := T4240RDB
  DEVICE_DTS_DIR := $(DTS_DIR)/fsl
  DEVICE_PACKAGES := \
    kmod-eeprom-at24 kmod-gpio-button-hotplug kmod-hwmon-w83793 kmod-leds-gpio \
	  kmod-ptp-qoriq kmod-rtc-ds1374
  FILESYSTEMS := squashfs
  KERNEL := kernel-bin | gzip | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
endef

define Device/fsl_t4240rdb-nor
  $(Device/fsl_t4240rdb)
  DEVICE_MODEL := T4240RDB (SDCARD BOOT)
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := \
    append-kernel | append-rootfs | pad-rootfs | append-metadata
endef
TARGET_DEVICES += fsl_t4240rdb-nor

define Device/fsl_t4240rdb-sdboot
  $(Device/fsl_t4240rdb)
  DEVICE_MODEL := T4240RDB (NOR BOOT)
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += fsl_t4240rdb-sdboot

define Device/watchguard_firebox-m300
  DEVICE_VENDOR := WatchGuard
  DEVICE_MODEL := Firebox M300
  DEVICE_COMPAT_MESSAGE := \
	Kernel switched to FIT uImage. Update U-Boot environment.
  DEVICE_COMPAT_VERSION := 1.1
  DEVICE_DTS_DIR := $(DTS_DIR)/fsl
  DEVICE_PACKAGES := \
	kmod-gpio-button-hotplug kmod-hwmon-w83793 kmod-leds-gpio kmod-ptp-qoriq \
	kmod-rtc-rs5c372a kmod-tpm-i2c-atmel kmod-dsa-mv88e6xxx
  KERNEL := kernel-bin | gzip | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_SUFFIX := -fit-uImage.itb
  IMAGES := sdcard.img.gz sysupgrade.img.gz
  IMAGE/sysupgrade.img.gz :=  sdcard-img | gzip | append-metadata
  IMAGE/sdcard.img.gz := sdcard-img | gzip
endef
TARGET_DEVICES += watchguard_firebox-m300
