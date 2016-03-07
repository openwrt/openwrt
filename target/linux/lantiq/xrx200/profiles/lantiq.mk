define Profile/EASY80920NOR
  NAME:=Lantiq VR9 - EASY80920NOR
  PACKAGES:=kmod-ath9k wpad-mini kmod-usb-dwc2 dsl-vrx200-firmware-xdsl-a dsl-vrx200-firmware-xdsl-b-patch
endef

$(eval $(call Profile,EASY80920NOR))

define Profile/EASY80920NAND
  NAME:=Lantiq VR9 - EASY80920NAND
  PACKAGES:=kmod-ath9k wpad-mini kmod-usb-dwc2 dsl-vrx200-firmware-xdsl-a dsl-vrx200-firmware-xdsl-b-patch
endef

$(eval $(call Profile,EASY80920NAND))
