# SPDX-License-Identifier: GPL-2.0-only

define Device/generic
  DEVICE_VENDOR := Generic
  DEVICE_MODEL := Octeon
  FILESYSTEMS := ext4
endef
TARGET_DEVICES += generic

# Disable PCIe on ER as it doesn't have PCIe peripherals and some devices lock up on initialization
ER_CMDLINE:=-mtdparts=phys_mapped_flash:640k(boot0)ro,640k(boot1)ro,64k(eeprom)ro root=/dev/mmcblk0p2 rootfstype=squashfs,ext4 rootwait pcie_octeon.pcie_disable=1
define Device/ubnt_edgerouter
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := EdgeRouter
  BOARD_NAME := er
  CMDLINE := $(ER_CMDLINE)
  SUPPORTED_DEVICES += er
endef
TARGET_DEVICES += ubnt_edgerouter

define Device/cisco_vedge1000
  DEVICE_VENDOR := Cisco Viptela
  DEVICE_MODEL := vEdge 1000
  BOARD_NAME := cisco,vedge1000
  DEVICE_PACKAGES += \
	blkid \
	kmod-hwmon-jc42 \
	kmod-hwmon-max6697 \
	kmod-of-mdio \
	kmod-rtc-ds1307 \
	kmod-usb-dwc3 \
	kmod-usb-storage-uas \
	kmod-usb3 \
	sfdisk \
	uboot-envtools
  KERNEL := kernel-bin | append-dtb-elf
  KERNEL_DEPENDS := $$(wildcard $(DTS_DIR)/$(DEVICE_DTS).dts)
  DEVICE_DTS := cn6130_cisco_vedge1000
endef
TARGET_DEVICES += cisco_vedge1000
