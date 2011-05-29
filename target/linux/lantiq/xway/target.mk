ARCH:=mips
SUBTARGET:=xway
BOARDNAME:=Xway
FEATURES:=squashfs jffs2 atm

DEFAULT_PACKAGES+=kmod-pppoa ppp-mod-pppoa linux-atm atm-tools br2684ctl kmod-ltq-dsl ltq-dsl-app swconfig

define Target/Description
	Lantiq XWAY (danube/twinpass/ar9)
endef
