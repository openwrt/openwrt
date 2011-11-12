define Profile/WBMR
  NAME:=WBMR - Buffalo WBMR-HP-G300H
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg kmod-ledtrig-usbdev \
	 kmod-ltq-dsl-firmware-b-ar9
endef

define Profile/WBMR/Description
	Package set optimized for the Buffalo WBMR routers
endef

$(eval $(call Profile,WBMR))
