define Profile/ARV752DPW22
  NAME:=ARV752DPW22 - Arcor Easybox 803
  PACKAGES:= kmod-usb-core kmod-usb2 kmod-usb-uhci kmod-usb-dwc-otg \
  	kmod-ledtrig-netdev kmod-ledtrig-usbdev kmod-leds-gpio kmod-button-hotplug \
	ltq-dsl-firmware-b
endef

define Profile/ARV752DPW22/Description
	Package set optimized for the ARV752PW22
endef

$(eval $(call Profile,ARV752DPW22))

