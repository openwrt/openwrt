define Profile/DGN3500B
  NAME:=DGN3500B Netgear
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg kmod-ath9k kmod-ltq-dsl-firmware-b-ar9
endef

define Profile/DGN3500B/Description
	Netgear DGN3500B
endef

$(eval $(call Profile,DGN3500B))

