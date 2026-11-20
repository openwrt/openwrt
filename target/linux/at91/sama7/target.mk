BOARDNAME:=SAMA7 boards (Cortex-A7)
CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=vfpv4
FEATURES+=fpu

define Target/Description
	Build generic firmware for Microchip SAMA7G5 MPUs using the
	ARMv7 instruction set.
endef
