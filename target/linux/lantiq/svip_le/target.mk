ARCH:=mipsel
SUBTARGET:=svip_le
BOARDNAME:=SVIP Little Endian
FEATURES:=squashfs
DEVICE_TYPE:=other

DEFAULT_PACKAGES+= uboot-svip hostapd-mini

define Target/Description
	Lantiq SVIP Little Endian
endef
