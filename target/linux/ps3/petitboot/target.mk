BOARDNAME:=Petitboot
FEATURES:=fpu

DEFAULT_PACKAGES+= dnsmasq

define Target/Description
	Build Petitboot bootloader
endef
