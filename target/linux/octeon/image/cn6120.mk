DTS_DIR := $(DTS_DIR)/octeon

# Disable PCIe on ER as it doesn't have PCIe peripherals and some devices lock up on initialization
ER_CMDLINE:=-mtdparts=phys_mapped_flash:640k(boot0)ro,640k(boot1)ro,64k(eeprom)ro root=/dev/mmcblk0p2 rootfstype=squashfs,ext4 rootwait pcie_octeon.pcie_disable=1
define Device/ubnt_edgerouter
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := EdgeRouter
  DEVICE_DTS_DIR := ../dts
  BOARD_NAME := er
  CMDLINE := $(ER_CMDLINE)
  SUPPORTED_DEVICES += er
endef
TARGET_DEVICES += ubnt_edgerouter

