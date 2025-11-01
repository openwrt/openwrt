SUBTARGET:=lan969x
BOARDNAME:=Microchip LAN969x switches
FEATURES+= boot-part rootfs-part

DEFAULT_PACKAGES += kmod-usb3 kmod-usb-dwc3

define Target/Description
	Build firmware images for Microchip LAN969x switch based boards.
endef
