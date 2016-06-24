ARCH:=mips
SUBTARGET:=xway_legacy
BOARDNAME:=XWAY Legacy
FEATURES:=squashfs atm mips16 ramdisk small_flash
CPU_TYPE:=34kc
CPU_SUBTYPE:=dsp

DEFAULT_PACKAGES+=kmod-leds-gpio kmod-gpio-button-hotplug

define Target/Description
	Lantiq XWAY Legacy for old boards with small flash
endef
