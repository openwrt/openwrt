ARCH:=mips
SUBTARGET:=xrx200
BOARDNAME:=XRX200
FEATURES:=squashfs atm mips16 jffs2 nand ubifs
CPU_TYPE:=34kc
CPU_SUBTYPE:=dsp

DEFAULT_PACKAGES+=kmod-leds-gpio \
	kmod-gpio-button-hotplug \
	ltq-vdsl-vr9-fw-installer \
	kmod-ltq-vdsl-vr9-mei \
	kmod-ltq-vdsl-vr9 \
	kmod-ltq-atm-vr9 \
	kmod-ltq-ptm-vr9 \
	kmod-ltq-deu-vr9 \
	ltq-vdsl-app \
	ppp-mod-pppoa \
	swconfig \
	atm-esi

define Target/Description
	Lantiq XRX200
endef
