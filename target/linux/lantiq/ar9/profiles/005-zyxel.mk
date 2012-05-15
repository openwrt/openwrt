define Profile/P2601HNFX
  NAME:=P2601HNFX - ZyXEL P-2601HN-Fx
  PACKAGES:= kmod-usb-dwc-otg kmod-rt2800-usb wpad-mini \
	kmod-ltq-dsl-firmware-b-ar9
endef

$(eval $(call Profile,P2601HNFX))
