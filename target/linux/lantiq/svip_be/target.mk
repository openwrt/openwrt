ARCH:=mips
SUBTARGET:=svip_be
BOARDNAME:=SVIP Big Endian
FEATURES:=squashfs
DEVICE_TYPE:=other

DEFAULT_PACKAGES+= uboot-svip hostapd-mini

define Target/Description
	Lantiq SVIP Big Endian
endef
