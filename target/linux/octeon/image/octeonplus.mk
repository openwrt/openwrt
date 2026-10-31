# SPDX-License-Identifier: GPL-2.0-only

ERLITE_CMDLINE:=-mtdparts=phys_mapped_flash:512k(boot0)ro,512k(boot1)ro,64k(eeprom)ro root=/dev/sda2 rootfstype=squashfs,ext4 rootwait
define Device/ubnt_edgerouter-lite
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := EdgeRouter Lite
  DEVICE_DTS := cn5020_ubnt_edgerouter-lite
  KERNEL := kernel-bin | patch-cmdline | append-dtb-to-elf
  KERNEL_DEPENDS := $$(wildcard $(DTS_DIR)/$(DEVICE_DTS).dts)
  CMDLINE := $(ERLITE_CMDLINE)
  SUPPORTED_DEVICES += erlite ubnt,erlite
endef
TARGET_DEVICES += ubnt_edgerouter-lite

define Device/ubnt_unifi-usg
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := UniFi Security Gateway
  BOARD_NAME := ubnt,usg
  DEVICE_PACKAGES += kmod-gpio-button-hotplug kmod-leds-gpio
  DEVICE_DTS := cn5020_ubnt_usg
  CMDLINE := $(ERLITE_CMDLINE)
  KERNEL += | append-dtb-to-elf
  SUPPORTED_DEVICES += ubnt,usg
endef
TARGET_DEVICES += ubnt_unifi-usg
