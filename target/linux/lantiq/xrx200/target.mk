ARCH:=mips
SUBTARGET:=xrx200
BOARDNAME:=XRX200
FEATURES:=squashfs atm mips16
CFLAGS+= -mtune=34kc -mdsp

DEFAULT_PACKAGES+=kmod-leds-gpio kmod-gpio-button-hotplug

define Target/Description
	Lantiq XRX200
endef
