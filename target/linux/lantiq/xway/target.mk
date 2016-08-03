ARCH:=mips
SUBTARGET:=xway
BOARDNAME:=XWAY
FEATURES:=squashfs atm mips16 nand ubifs ramdisk
CPU_TYPE:=34kc

DEFAULT_PACKAGES+=kmod-leds-gpio kmod-gpio-button-hotplug

define Target/Description
	Lantiq XWAY
endef
