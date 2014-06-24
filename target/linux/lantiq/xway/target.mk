ARCH:=mips
SUBTARGET:=xway
BOARDNAME:=XWAY
FEATURES:=squashfs atm mips16 nand ubifs
CPU_TYPE:=34kc
CPU_SUBTYPE:=dsp

DEFAULT_PACKAGES+=kmod-leds-gpio kmod-gpio-button-hotplug atm-esi

define Target/Description
	Lantiq XWAY
endef
