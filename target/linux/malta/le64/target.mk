ARCH:=mips64el
ARCH_PACKAGES:=malta_mipsel64
SUBTARGET:=le64
BOARDNAME:=Little Endian (64-bits)
FEATURES:=ramdisk
CFLAGS:=-Os -pipe -mips64 -mtune=mips64

define Target/Description
	Build LE firmware images for MIPS Malta CoreLV board running in
	little-endian and 64-bits mode
endef
