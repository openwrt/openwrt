#
# MT7621 (small flash) Profiles
#

DEFAULT_SOC := mt7621

define Device/dsa-migration
  DEVICE_COMPAT_VERSION := 1.1
  DEVICE_COMPAT_MESSAGE := Config cannot be migrated from swconfig to DSA
endef
