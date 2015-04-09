define Profile/MicroWRT
	NAME:=Microduino MicroWRT
	PACKAGES:= kmod-usb-core kmod-usb2 kmod-usb-ohci
endef

define Profile/MicroWRT/Description
		Package set compatible with MicroWRT
endef
$(eval $(call Profile,MicroWRT))
