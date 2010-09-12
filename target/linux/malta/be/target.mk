ARCH:=mips
ARCH_PACKAGES:=malta_mips
SUBTARGET:=be
BOARDNAME:=Big Endian
FEATURES:=ramdisk

define Target/Description
	Build BE firmware images for MIPS Malta CoreLV board running in
	big-endian mode
endef
