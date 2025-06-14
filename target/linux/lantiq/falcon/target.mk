ARCH:=mips
SUBTARGET:=falcon
BOARDNAME:=Falcon
FEATURES+=nand
CPU_TYPE:=24kc

DEFAULT_PACKAGES+= kmod-leds-gpio \
	kmod-gpio-button-hotplug

define Target/Description
	Lantiq Falcon
endef
