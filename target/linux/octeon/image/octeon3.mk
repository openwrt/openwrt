# SPDX-License-Identifier: GPL-2.0-only

define Device/ubnt_edgerouter-e300
  DEVICE_VENDOR := Ubiquiti
  DEVICE_PACKAGES += kmod-gpio-button-hotplug kmod-leds-gpio kmod-of-mdio kmod-sfp kmod-usb3 kmod-usb-dwc3 kmod-usb-storage-uas
  KERNEL := kernel-bin | patch-cmdline | append-dtb-to-elf
  KERNEL_DEPENDS := $$(wildcard $(DTS_DIR)/$(DEVICE_DTS).dts)
  CMDLINE := root=/dev/mmcblk0p2 rootfstype=squashfs,ext4 rootwait
endef

define Device/ubnt_edgerouter-4
  $(Device/ubnt_edgerouter-e300)
  DEVICE_MODEL := EdgeRouter 4
  DEVICE_DTS := cn7130_ubnt_edgerouter-4
endef
TARGET_DEVICES += ubnt_edgerouter-4

define Device/ubnt_edgerouter-6p
  $(Device/ubnt_edgerouter-e300)
  DEVICE_MODEL := EdgeRouter 6P
  DEVICE_DTS := cn7130_ubnt_edgerouter-6p
endef
TARGET_DEVICES += ubnt_edgerouter-6p

ITUSROUTER_CMDLINE:=console=ttyS0,115200 root=/dev/mmcblk1p2 rootfstype=squashfs,ext4,f2fs rootwait
define Device/itus_shield-router
  DEVICE_VENDOR := Itus Networks
  DEVICE_MODEL := Shield Router
  CMDLINE := $(ITUSROUTER_CMDLINE)
  IMAGE/sysupgrade.tar/squashfs += | append-metadata
endef
TARGET_DEVICES += itus_shield-router
