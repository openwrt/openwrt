ARCH:=mips
SUBTARGET:=be
BOARDNAME:=Big Endian
FEATURES:=ramdisk mips16

define Target/Description
	Build BE firmware images for MIPS Malta CoreLV board running in
	big-endian mode
endef
