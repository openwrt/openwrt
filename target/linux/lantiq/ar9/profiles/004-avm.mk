define Profile/FRITZ7320
  NAME:=FRITZ7320 Fritzbox
  PACKAGES:= kmod-usb-core kmod-usb-ifxhcd kmod-ledtrig-usbdev \
	 kmod-ltq-dsl-firmware-b-ar9 kmod-ath9k wpad-mini
endef

$(eval $(call Profile,FRITZ7320))
