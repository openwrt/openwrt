ARCH:=mips
SUBTARGET:=xway
BOARDNAME:=XWAY
FEATURES:=squashfs jffs2 atm

DEFAULT_PACKAGES+=kmod-leds-gpio kmod-button-hotplug kmod-input-gpio-keys-polled

define Target/Description
	Lantiq XWAY
endef
