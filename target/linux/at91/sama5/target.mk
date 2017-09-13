BOARDNAME:=SAMA5 Xplained boards(Cortex-A5)
CPU_TYPE:=cortex-a5

DEFAULT_PACKAGES += kmod-usb2

define Target/Description
	Build generic firmware for Microchip(Atmel AT91) SAMA5 MPU's
	using the ARMv7 instruction set.
endef
