define Profile/ARV7525PW
  NAME:=Speedport W303V - ARV7525PW
endef

$(eval $(call Profile,ARV7525PW))

define Profile/ARV4520PW
  NAME:=Easybox 800, WAV-281 - ARV4520PW
endef

$(eval $(call Profile,ARV4520PW))

define Profile/ARV752DPW
  NAME:=Easybox 802 - ARV752DPW
endef

$(eval $(call Profile,ARV752DPW))

define Profile/ARV752DPW22
  NAME:=Easybox 803 - ARV752DPW22
  PACKAGES:= kmod-usb-core kmod-usb2 kmod-usb-uhci kmod-usb-dwc-otg kmod-ledtrig-usbdev \
	kmod-ltq-dsl-firmware-b-danube
endef

$(eval $(call Profile,ARV752DPW22))
