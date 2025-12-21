SUBTARGET:=lan969x
BOARDNAME:=Microchip LAN969x switches
FEATURES+= boot-part rootfs-part

DEFAULT_PACKAGES += kmod-usb3 kmod-usb-dwc3 \
	e2fsprogs kmod-fs-ext4 losetup \
	kmod-fs-f2fs f2fs-tools

define Target/Description
	Build firmware images for Microchip LAN969x switch based boards.
endef
