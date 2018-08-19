define Profile/ANONABOX_PRO
	NAME:=Anonabox Pro
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage
endef

define Profile/ANONABOX_PRO/Description
	Package set optimized for the Anonabox Pro.
endef

$(eval $(call Profile,ANONABOX_PRO))

