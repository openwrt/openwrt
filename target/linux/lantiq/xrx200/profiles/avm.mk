define Profile/FRITZ3370
  NAME:=Fritz!Box WLan - FRITZ3370
  PACKAGES:=kmod-ath9k wpad-mini kmod-usb-dwc2
endef

$(eval $(call Profile,FRITZ3370))

define Profile/FRITZ7360SL
  NAME:=1&1 HomeServer - FRITZ7360SL
  PACKAGES:=kmod-ath9k wpad-mini kmod-usb-dwc2
endef

$(eval $(call Profile,FRITZ7360SL))
