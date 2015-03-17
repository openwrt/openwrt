define Profile/VG3503J
  NAME:=BT Openreach - ECI VDSL Modem
endef

$(eval $(call Profile,VG3503J))

define Profile/VG3503J_V2
  NAME:=BT Openreach - ECI VDSL Modem v2
endef

$(eval $(call Profile,VG3503J_V2))

define Profile/VGV7519NOR
  NAME:=Experiabox 8 VGV7519
  PACKAGES:=kmod-rt2800-pci wpad-mini kmod-usb-dwc2
endef

$(eval $(call Profile,VGV7519NOR))

define Profile/VGV7519BRN
  NAME:=Experiabox 8 VGV7519 (BRN)
  PACKAGES:=kmod-rt2800-pci wpad-mini kmod-usb-dwc2
endef

$(eval $(call Profile,VGV7519BRN))

define Profile/ARV7519RW22
  NAME:=Livebox Astoria ARV7519RW22
  PACKAGES:=kmod-usb-dwc2
endef

$(eval $(call Profile,ARV7519RW22))
