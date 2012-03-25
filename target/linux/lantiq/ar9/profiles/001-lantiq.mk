define Profile/EASY50812
  NAME:=EASY50812 - Eval Board
  PACKAGES:= kmod-usb-core kmod-usb-dwc-otg
endef

define Profile/EASY50812/Description
	Lantiq EASY50812 evalkit
endef

$(eval $(call Profile,EASY50812))
