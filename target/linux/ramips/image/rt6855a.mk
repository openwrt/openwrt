# SPDX-License-Identifier: GPL-2.0-only

define Device/zyxel_keenetic-giga-ii
  SOC := rt6855a
  IMAGE_SIZE := 14976k
  BLOCKSIZE := 4k
  DEVICE_VENDOR := ZyXEL
  DEVICE_MODEL := Keenetic Giga II
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | check-size
  DEVICE_PACKAGES := kmod-rt2800-pci kmod-switch-rtl8367b \
	kmod-gpio-rt6856 kmod-leds-gpio kmod-gpio-button-hotplug \
	kmod-usb2 kmod-usb-ohci kmod-usb-storage \
	rt2800-pci-firmware swconfig
endef
TARGET_DEVICES += zyxel_keenetic-giga-ii
