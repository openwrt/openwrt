# SPDX-License-Identifier: GPL-2.0-only

SUBTARGET:=rt6855a
BOARDNAME:=RT6855A/RT6856 based boards
FEATURES+=ramdisk pci usb
CPU_TYPE:=24kc

DEFAULT_PACKAGES += wpad-basic-mbedtls swconfig

define Target/Description
	Build firmware images for Ralink RT6855A/RT6856 based boards.
endef
