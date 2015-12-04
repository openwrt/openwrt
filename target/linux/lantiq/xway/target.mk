ARCH:=mips
SUBTARGET:=xway
BOARDNAME:=XWAY
FEATURES:=squashfs atm mips16 nand ubifs ramdisk
CPU_TYPE:=34kc
CPU_SUBTYPE:=dsp

DEFAULT_PACKAGES+=kmod-leds-gpio kmod-gpio-button-hotplug

define Target/Description
	Lantiq XWAY
endef
