define Profile/WBMR
  NAME:=WBMR - Buffalo WBMR-HP-G300H
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg kmod-leds-gpio \
	 kmod-ltq-dsl-firmware-b-ar9 kmod-ledtrig-usbdev
endef

define Profile/WBMR/Description
	Package set optimized for the Buffalo WBMR routers
endef

$(eval $(call Profile,WBMR))
