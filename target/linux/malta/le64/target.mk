ARCH:=mips64el
ARCH_PACKAGES:=malta_mipsel64
SUBTARGET:=le64
BOARDNAME:=Little Endian (64-bits)
CFLAGS:=-Os -pipe -mips64 -mtune=mips64
FEATURES:=ramdisk

define Target/Description
	Build LE firmware images for MIPS Malta CoreLV board running in
	little-endian and 64-bits mode.
endef
