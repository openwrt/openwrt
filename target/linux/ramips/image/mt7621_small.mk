#
# MT7621 (small flash) Profiles
#

DEFAULT_SOC := mt7621

define Device/dsa-migration
  DEVICE_COMPAT_VERSION := 1.1
  DEVICE_COMPAT_MESSAGE := Config cannot be migrated from swconfig to DSA
endef

define Device/cudy_re1800-v2
  $(Device/dsa-migration)
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := RE1800
  DEVICE_VARIANT := v2
  IMAGE_SIZE := 7808k
  UIMAGE_NAME := R27
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  SUPPORTED_DEVICES += R27
endef
TARGET_DEVICES += cudy_re1800-v2
