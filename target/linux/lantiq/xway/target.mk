ARCH:=mips
SUBTARGET:=xway
BOARDNAME:=XWAY
FEATURES:=squashfs jffs2 atm

DEFAULT_PACKAGES+=swconfig kmod-leds-gpio kmod-button-hotplug

define Target/Description
	Lantiq XWAY
endef
