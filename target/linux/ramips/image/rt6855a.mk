# SPDX-License-Identifier: GPL-2.0-only

define Build/pad-to-ff
	let \
		size="$$(stat -c%s $@)" \
		target="$(call exp_units,$(1))" \
		pad='target - size'; \
		[ "$$pad" -ge 0 ] || { echo "image exceeds padding target" >&2; exit 1; }; \
		if [ "$$pad" -gt 0 ]; then \
			dd if=/dev/zero bs=$$pad count=1 2>/dev/null | \
				tr "\000" "\377" >> $@; \
		fi
endef

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

define Device/zyxel_keenetic-ultra
  SOC := rt6855a
  IMAGE_SIZE := 14912k
  BLOCKSIZE := 4k
  DEVICE_VENDOR := ZyXEL
  DEVICE_MODEL := Keenetic Ultra
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | pad-to 64k | check-size | \
	pad-to-ff $$$$(IMAGE_SIZE) | \
	zyimage -d 22037 -v "ZyXEL Keenetic Ultra"
  SUPPORTED_DEVICES += ku_ra
  DEVICE_PACKAGES := kmod-rt2800-pci kmod-switch-rtl8367b \
	kmod-gpio-rt6856 kmod-leds-gpio kmod-gpio-button-hotplug \
	kmod-usb2 kmod-usb-ohci kmod-usb-storage kmod-usb-ledtrig-usbport \
	rt2800-pci-firmware swconfig
endef
TARGET_DEVICES += zyxel_keenetic-ultra
