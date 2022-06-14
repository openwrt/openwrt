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
