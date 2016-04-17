ARCH:=mipsel
SUBTARGET:=le
BOARDNAME:=Little Endian
FEATURES:=ramdisk mips16

define Target/Description
	Build LE firmware images for MIPS Malta CoreLV board running in
	little-endian mode
endef
