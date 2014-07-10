define Profile/ACMP252
  NAME:=AudioCodes MediaPack MP-252
  PACKAGES:=kmod-rt2x00-pci wpad-mini \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-ptm-danube \
	ltq-adsl-app
endef

$(eval $(call Profile,ACMP252))
