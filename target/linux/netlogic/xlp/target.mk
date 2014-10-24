CPU_TYPE:=xlp
CPU_FLAGS_xlp:=-march=xlp -mabi=64
BOARDNAME:=xlp

DEFAULT_PACKAGES += kmod-usb-core kmod-usb2 kmod-usb2-pci kmod-usb3 \
		    kmod-usb-net kmod-usb-net-smsc95xx

define Target/Description
	Build firmware images for Broadcom/Netlogic XLP boards.
endef
