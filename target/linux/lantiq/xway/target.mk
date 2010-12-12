ARCH:=mips
SUBTARGET:=xway
BOARDNAME:=Xway
FEATURES:=squashfs jffs2 atm

DEFAULT_PACKAGES+=uboot-lantiq-easy50712 kmod-pppoa ppp-mod-pppoa linux-atm atm-tools br2684ctl ifxmips-dsl-api ifxmips-dsl-control ifx-tapidemo

define Target/Description
	Lantiq XWAY (danube/twinpass/ar9)
endef

