define Profile/TDW8970
  NAME:=TP-LINK TD-W8970
  PACKAGES:=kmod-ath9k wpad-mini kmod-usb-dwc2 kmod-ledtrig-usbdev
endef

$(eval $(call Profile,TDW8970))
