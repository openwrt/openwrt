# SPDX-License-Identifier: GPL-2.0-only

define Device/zyxel_keenetic-giga-ii
  SOC := rt6855a
  IMAGE_SIZE := 14976k
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := Keenetic Giga II
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | check-size | \
	zyimage -d 0x6215 -v "ZyXEL Keenetic Giga II"
  DEVICE_PACKAGES := kmod-rt2800-pci kmod-switch-rtl8367b \
	kmod-usb2 kmod-usb-ohci kmod-usb-storage \
	rt2800-pci-firmware
endef
TARGET_DEVICES += zyxel_keenetic-giga-ii
