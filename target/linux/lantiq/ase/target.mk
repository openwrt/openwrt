ARCH:=mips
SUBTARGET:=ase
BOARDNAME:=Amazon-SE
FEATURES:=squashfs atm
CFLAGS+= -mtune=mips32r2

DEFAULT_PACKAGES+=kmod-pppoa ppp-mod-pppoa linux-atm atm-tools br2684ctl kmod-ltq-dsl-ase ltq-dsl-app

define Target/Description
	Lantiq ASE
endef
