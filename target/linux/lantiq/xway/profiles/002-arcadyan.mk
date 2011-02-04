define Profile/ARV4518PW
  NAME:=ARV4518PW - SMC7908A
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg \
  	kmod-ledtrig-netdev kmod-ledtrig-usbdev kmod-leds-gpio kmod-button-hotplug \
	kmod-madwifi wpad-mini ltq-dsl-firmware-a
endef

define Profile/ARV4518PW/Description
	Package set optimized for the ARV4518PW
endef

$(eval $(call Profile,ARV4518PW))

define Profile/ARV4520PW
  NAME:=ARV4520PW - Arcor Easybox 800
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg \
  	kmod-ledtrig-netdev kmod-ledtrig-usbdev kmod-leds-gpio kmod-button-hotplug \
	kmod-rt61-pci wpad-mini ltq-dsl-firmware-a 
endef

define Profile/ARV4520PW/Description
	Package set optimized for the ARV4520PW
endef

$(eval $(call Profile,ARV4520PW))

define Profile/ARV4525PW
  NAME:=ARV4525PW - Speedport W502V
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg \
  	kmod-ledtrig-netdev kmod-ledtrig-usbdev kmod-leds-gpio kmod-button-hotplug \
	kmod-madwifi wpad-mini ltq-dsl-firmware-b
endef

define Profile/ARV4525PW/Description
	Package set optimized for the ARV4525PW
endef

$(eval $(call Profile,ARV4525PW))

define Profile/ARV452CPW
  NAME:=ARV452CPW - Arcor Easybox 801
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg \
  	kmod-ledtrig-netdev kmod-ledtrig-usbdev kmod-leds-gpio kmod-button-hotplug \
	kmod-madwifi wpad-mini ltq-dsl-firmware-b
endef

define Profile/ARV452CPW/Description
	Package set optimized for the ARV452CPW
endef

$(eval $(call Profile,ARV452CPW))

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

