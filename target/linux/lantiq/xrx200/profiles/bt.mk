define Profile/BTHOMEHUBV5A
  NAME:=BT Home Hub 5A
  PACKAGES:=kmod-ath9k kmod-ath10k wpad-mini kmod-usb-dwc2
endef

$(eval $(call Profile,BTHOMEHUBV5A))
