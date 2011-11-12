define Profile/EASY50712
  NAME:=EASY50712
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg
endef

define Profile/EASY50712/Description
	Lantiq EASY50712 evalkit
endef

$(eval $(call Profile,EASY50712))
