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
  DEVICE_DTS_DIR := $(DTS_DIR)/cavium-octeon
endef
TARGET_DEVICES += cisco_vedge1000

