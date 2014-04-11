BOARDNAME:=Intel EP80579
LINUX_VERSION:=3.3.8
DEFAULT_PACKAGES += kmod-ep80579-can kmod-ep80579-eth kmod-ep80579-misc \
			kmod-usb-core kmod-usb2 kmod-usb-hid kmod-usb-uhci

define Target/Description
	Build firmware images for Intel EP80579 based boards
	(e.g : ADI Engineering Ocracoke Island)
endef

