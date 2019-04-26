ARCH:=aarch64
SUBTARGET:=64
BOARDNAME:=ARMv8 multiplatform
KERNELNAME:=Image
DEFAULT_PACKAGES += kmod-fs-9p

define Target/Description
	Build multi-platform images for the ARMv8 instruction set architecture
endef
