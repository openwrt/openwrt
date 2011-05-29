define Profile/EASY50601
  NAME:=EASY50601
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg kmod-leds-gpio
endef

define Profile/EASY50601/Description
	Lantiq EASY50601 evalkit
endef

$(eval $(call Profile,EASY50601))
