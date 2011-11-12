ARCH:=mips
SUBTARGET:=ar9
BOARDNAME:=AR9
FEATURES:=squashfs jffs2 atm

DEFAULT_PACKAGES+=kmod-pppoa ppp-mod-pppoa linux-atm atm-tools br2684ctl kmod-ltq-dsl-ar9 ltq-dsl-app \
	kmod-input-gpio-keys-polled kmod-ledtrig-netdev kmod-leds-gpio kmod-button-hotplug \
	swconfig

define Target/Description
	Lantiq AR9
endef
