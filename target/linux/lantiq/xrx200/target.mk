ARCH:=mips
SUBTARGET:=xrx200
BOARDNAME:=XRX200
FEATURES:=squashfs atm mips16 jffs2 ubifs
CPU_TYPE:=34kc
CPU_SUBTYPE:=dsp

DEFAULT_PACKAGES+=kmod-leds-gpio kmod-gpio-button-hotplug

define Target/Description
	Lantiq XRX200
endef
