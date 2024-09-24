ERLITE_CMDLINE:=-mtdparts=phys_mapped_flash:512k(boot0)ro,512k(boot1)ro,64k(eeprom)ro root=/dev/sda2 rootfstype=squashfs,ext4 rootwait
define Device/ubnt_edgerouter-lite
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := EdgeRouter Lite
  DEVICE_DTS_DIR := $(DTS_DIR)/dts
  BOARD_NAME := erlite
  CMDLINE := $(ERLITE_CMDLINE)
  SUPPORTED_DEVICES += erlite
endef
TARGET_DEVICES += ubnt_edgerouter-lite

define Device/ubnt_unifi-usg
  $(Device/ubnt_edgerouter-lite)
  DEVICE_MODEL := UniFi Security Gateway
  BOARD_NAME := ubnt,usg
  DEVICE_PACKAGES += kmod-gpio-button-hotplug kmod-leds-gpio
  DEVICE_DTS := cn5020_ubnt_usg
  DEVICE_DTS_DIR := $(DTS_DIR)/cavium-octeon
  KERNEL += | append-dtb-to-elf
endef
TARGET_DEVICES += ubnt_unifi-usg

