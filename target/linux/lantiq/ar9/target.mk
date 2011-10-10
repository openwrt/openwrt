ARCH:=mips
SUBTARGET:=ar9
BOARDNAME:=AR9
FEATURES:=squashfs jffs2 atm

DEFAULT_PACKAGES+=kmod-pppoa ppp-mod-pppoa linux-atm atm-tools br2684ctl kmod-ltq-dsl-ar9 ltq-dsl-app swconfig

define Target/Description
	Lantiq XWAY (danube/twinpass/ar9)
endef
