define Profile/EASY80920NOR
  NAME:=Lantiq VR9 - EASY80920NOR
  PACKAGES:=kmod-ath9k wpad-mini kmod-usb-dwc2
endef

$(eval $(call Profile,EASY80920NOR))

define Profile/EASY80920NAND
  NAME:=Lantiq VR9 - EASY80920NAND
  PACKAGES:=kmod-ath9k wpad-mini kmod-usb-dwc2
endef

$(eval $(call Profile,EASY80920NAND))
