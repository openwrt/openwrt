SUBTARGET:=ipq95xx
FEATURES += source-only
BOARDNAME:=Qualcomm Atheros IPQ95xx
CPU_TYPE:=cortex-a73

define Target/Description
	Build firmware images for Qualcomm Atheros IPQ95xx based boards.
endef

DEFAULT_PACKAGES+= -kmod-qca-nss-dp -kmod-ath11k-ahb -wpad-basic-mbedtls uboot-envtools
