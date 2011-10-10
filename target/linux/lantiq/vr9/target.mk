ARCH:=mips
SUBTARGET:=vr9
BOARDNAME:=VR9
FEATURES:=squashfs jffs2 atm

DEFAULT_PACKAGES+=kmod-pppoa ppp-mod-pppoa linux-atm atm-tools br2684ctl kmod-ltq-dsl-vr9 ltq-dsl-app swconfig

define Target/Description
	Lantiq VR9
endef
