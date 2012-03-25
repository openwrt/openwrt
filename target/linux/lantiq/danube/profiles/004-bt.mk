define Profile/BTHOMEHUBV2B
  NAME:=BTHOMEHUBV2B - BT Homehub V2.0 Type B
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg kmod-ledtrig-usbdev \
	kmod-ath9k wpad-mini kmod-ltq-dsl-firmware-a-danube
endef

define Profile/BTHOMEHUBV2B/Description
	Package set optimized for the BT Homehub V2.0 Type B using whole nand for OpenWRT
endef

$(eval $(call Profile,BTHOMEHUBV2B))

define Profile/BTHOMEHUBV2BOPENRG
  NAME:=BTHOMEHUBV2B - BT Homehub V2.0 Type B (OpenRG)
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg kmod-ledtrig-usbdev \
	kmod-ath9k wpad-mini kmod-ltq-dsl-firmware-a-danube
endef

define Profile/BTHOMEHUBV2BOPENRG/Description
	Package set optimized for the BT Homehub V2.0 Type B but retaining OpenRG image
endef

$(eval $(call Profile,BTHOMEHUBV2BOPENRG))
