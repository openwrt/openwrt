ARCH:=mips64
ARCH_PACKAGES:=malta_mips64
SUBTARGET:=be64
BOARDNAME:=Big Endian (64-bits)
FEATURES:=ramdisk

define Target/Description
	Build BE firmware images for MIPS Malta CoreLV board running in
	big-endian and 64-bits mode
endef
