BOARDNAME:=Petitboot
FEATURES:=fpu ramdisk

DEFAULT_PACKAGES+= dnsmasq petitboot

define Target/Description
	Build Petitboot bootloader
endef
