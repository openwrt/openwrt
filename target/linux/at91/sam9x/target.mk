BOARDNAME:= SAM9X Boards (ARMv5)
CPU_TYPE:=arm926ej-s

DEFAULT_PACKAGES += kmod-usb-ohci kmod-at91-udc kmod-usb-gadget-eth

define Target/Description
	Build generic firmware for Microchip AT91 SAM9x platforms
	using the ARMv5 instruction set.
endef
