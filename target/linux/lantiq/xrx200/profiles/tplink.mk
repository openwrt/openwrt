define Profile/TDW8970
  NAME:=TP-LINK TD-W8970
  PACKAGES:=kmod-ath9k wpad-mini \
	kmod-ltq-deu-vr9 kmod-usb-dwc2 \
	kmod-ltq-vdsl-vr9-mei kmod-ltq-vdsl-vr9 \
	kmod-ltq-atm-vr9 ltq-vdsl-vr9-fw-installer \
	ltq-vdsl-app ppp-mod-pppoa \
	kmod-ledtrig-usbdev
endef

$(eval $(call Profile,TDW8970))
