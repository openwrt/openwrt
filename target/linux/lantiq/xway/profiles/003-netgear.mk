define Profile/DGN3500B
  NAME:=DGN3500B
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg kmod-leds-gpio
endef

define Profile/DGN3500B/Description
	Netgear DGN3500B
endef

$(eval $(call Profile,DGN3500B))

