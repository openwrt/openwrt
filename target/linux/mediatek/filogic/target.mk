ARCH:=aarch64
SUBTARGET:=filogic
BOARDNAME:=Filogic 8x0 (MT798x)
CPU_TYPE:=cortex-a53
DEFAULT_PACKAGES += \
	fitblk kmod-crypto-hw-safexcel kmod-nf-conntrack-bridge \
	wpad-basic-mbedtls uboot-envtools
KERNELNAME:=Image dtbs
DEFAULT_PROFILE:=openwrt_one

define Target/Description
	Build firmware images for MediaTek Filogic ARM based boards.
endef
