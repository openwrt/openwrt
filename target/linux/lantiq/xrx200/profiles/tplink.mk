define Profile/TDW8970
  NAME:=TP-LINK TD-W8970
  PACKAGES:=kmod-ath9k wpad-mini kmod-usb-dwc2 kmod-ledtrig-usbdev dsl-vrx200-firmware-xdsl-a dsl-vrx200-firmware-xdsl-b-patch
endef

$(eval $(call Profile,TDW8970))

define Profile/TDW8980
  NAME:=TP-LINK TD-W8980
  PACKAGES:=kmod-ath9k wpad-mini kmod-usb-dwc2 kmod-ledtrig-usbdev dsl-vrx200-firmware-xdsl-a dsl-vrx200-firmware-xdsl-b-patch
endef

$(eval $(call Profile,TDW8980))

define Profile/VR200v
  NAME:=TP-LINK Archer VR200v
  PACKAGES:=kmod-usb-dwc2 kmod-ledtrig-usbdev dsl-vrx200-firmware-xdsl-a dsl-vrx200-firmware-xdsl-b-patch
endef

$(eval $(call Profile,VR200v))
