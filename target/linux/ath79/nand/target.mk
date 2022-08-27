BOARDNAME := Generic devices with NAND flash

FEATURES += nand

DEFAULT_PACKAGES += wpad-basic-wolfssl

KERNEL_TESTING_PATCHVER:=5.15

define Target/Description
	Firmware for boards using Qualcomm Atheros, MIPS-based SoCs
	in the ar72xx and subsequent series, with support for NAND flash
endef
