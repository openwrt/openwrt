define Profile/GR7000
  NAME:=Aztech GR7000
  PACKAGES:= \
	kmod-ltq-deu-ar9 kmod-usb-dwc2 \
	swconfig
endef
$(eval $(call Profile,GR7000))
