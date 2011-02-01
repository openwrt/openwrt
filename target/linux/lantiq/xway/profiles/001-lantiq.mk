define Profile/EASY50712
  NAME:=EASY50712
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg kmod-leds-gpio
endef

define Profile/EASY50712/Description
	Lantiq EASY50712 evalkit
endef

$(eval $(call Profile,EASY50712))

define Profile/EASY50812
  NAME:=EASY50812
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg kmod-leds-gpio
endef

define Profile/EASY50812/Description
	Lantiq EASY50812 evalkit
endef

$(eval $(call Profile,EASY50812))

