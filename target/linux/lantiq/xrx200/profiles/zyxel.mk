define Profile/P2812HNUF1
  NAME:=ZyXEL P-2812HNU-F1
  PACKAGES:=kmod-rt2800-pci wpad-mini kmod-usb-dwc2
endef

$(eval $(call Profile,P2812HNUF1))

define Profile/P2812HNUF3
  NAME:=ZyXEL P-2812HNU-F3
  PACKAGES:=kmod-rt2800-pci wpad-mini kmod-usb-dwc2
endef

$(eval $(call Profile,P2812HNUF3))

