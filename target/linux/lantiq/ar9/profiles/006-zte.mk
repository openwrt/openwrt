define Profile/H201L
  NAME:=H201L - ZTE ZXV10
  PACKAGES:= kmod-usb-dwc-otg kmod-ath9k-htc wpad-mini \
	 kmod-ltq-dsl-firmware-b-ar9
endef

$(eval $(call Profile,H201L))
