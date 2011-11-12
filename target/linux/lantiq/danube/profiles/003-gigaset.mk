define Profile/GIGASX76X
  NAME:=GIGASX76X - Gigaset SX761,SX762,SX763
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg kmod-ledtrig-usbdev \
	 kmod-ltq-dsl-firmware-b-danube
endef

define Profile/GIGASX76X/Description
	Package set optimized for the Gigaset SX76X routers
endef

$(eval $(call Profile,GIGASX76X))
