define Device/fsl_t4240rdb
  DEVICE_VENDOR := NXP
  DEVICE_MODEL := T4240RDB
  DEVICE_DTS_DIR := $(DTS_DIR)/fsl
  DEVICE_PACKAGES := \
    kmod-eeprom-at24 kmod-gpio-button-hotplug kmod-hwmon-w83793 kmod-leds-gpio \
	  kmod-ptp-qoriq kmod-rtc-ds1374 layerscape-fman qoriq-cortina
  FILESYSTEMS := squashfs
  KERNEL := kernel-bin | gzip | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
endef

define Device/fsl_t4240rdb-nor
  $(Device/fsl_t4240rdb)
  DEVICE_MODEL := T4240RDB (SDCARD BOOT)
  DEVICE_PACKAGES += layerscape-rcw u-boot-fsl_t4240rdb-nor
  IMAGES := factory.bin.gz sysupgrade.bin rcw.bin
  IMAGE/factory.bin.gz := \
    append-kernel | append-rootfs | pad-rootfs | pad-to 126M | \
    ls-append cs4315-cs4340-PHY-ucode.txt | pad-to 127M | \
    ls-append fsl_t4240-rdb-fman.bin | pad-to 130176k | \
    ls-append $(1)-uboot-env.bin | pad-to 130304k | \
    ls-append $(1)-uboot.bin | gzip
  IMAGE/sysupgrade.bin := \
    append-kernel | append-rootfs | pad-rootfs | append-metadata
  IMAGE/rcw.bin := \
    ls-append $(1)-rcw.bin
endef
TARGET_DEVICES += fsl_t4240rdb-nor

define Device/fsl_t4240rdb-sdboot
  $(Device/fsl_t4240rdb)
  DEVICE_MODEL := T4240RDB (NOR BOOT)
  DEVICE_PACKAGES += u-boot-fsl_t4240rdb-sdboot
  IMAGES := sdcard.img.gz sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/sdcard.img.gz := \
    ls-clean | \
    ls-append-sdhead $(1) | pad-to 4K | \
    ls-append $(1)-uboot.bin | pad-to 1028k | \
    ls-append $(1)-uboot-env.bin | pad-to 1040k | \
    ls-append fsl_t4240-rdb-fman.bin | pad-to 2M | \
    ls-append cs4315-cs4340-PHY-ucode.txt | pad-to $(LS_SD_KERNELPART_OFFSET)M | \
    ls-append-kernel | pad-to $(LS_SD_ROOTFSPART_OFFSET)M | \
    append-rootfs | pad-to $(LS_SD_IMAGE_SIZE)M | gzip
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
	kmod-rtc-rs5c372a kmod-tpm-i2c-atmel
  KERNEL := kernel-bin | gzip | fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_SUFFIX := -fit-uImage.itb
  IMAGES := sdcard.img.gz sysupgrade.img.gz
  IMAGE/sysupgrade.img.gz :=  sdcard-img | gzip | append-metadata
  IMAGE/sdcard.img.gz := sdcard-img | gzip
endef
TARGET_DEVICES += watchguard_firebox-m300
