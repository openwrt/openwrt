define Profile/VG3503J
  NAME:=BT Openreach - ECI VDSL Modem
  PACKAGES:=kmod-ltq-vdsl-vr9-mei kmod-ltq-vdsl-vr9 \
       kmod-ltq-ptm-vr9 ltq-vdsl-app
endef

$(eval $(call Profile,VG3503J))

define Profile/VG3503J_V2
  NAME:=BT Openreach - ECI VDSL Modem v2
  PACKAGES:=kmod-ltq-vdsl-vr9-mei kmod-ltq-vdsl-vr9 \
       kmod-ltq-ptm-vr9 ltq-vdsl-app
endef

$(eval $(call Profile,VG3503J_V2))

define Profile/VGV7519NOR
  NAME:=Experiabox 8 VGV7519
  PACKAGES:=kmod-ath9k wpad-mini \
	kmod-ltq-deu-vr9 kmod-ltq-hcd-vr9 \
	kmod-ltq-vdsl-vr9-mei kmod-ltq-vdsl-vr9 \
	kmod-ltq-atm-vr9 ltq-vdsl-vr9-fw-installer \
	ltq-vdsl-app ppp-mod-pppoa
endef

$(eval $(call Profile,VGV7519NOR))

define Profile/VGV7519BRN
  NAME:=Experiabox 8 VGV7519 (BRN)
  PACKAGES:=kmod-ath9k wpad-mini \
	kmod-ltq-deu-vr9 kmod-ltq-hcd-vr9 \
	kmod-ltq-vdsl-vr9-mei kmod-ltq-vdsl-vr9 \
	kmod-ltq-atm-vr9 ltq-vdsl-vr9-fw-installer \
	ltq-vdsl-app ppp-mod-pppoa
endef

$(eval $(call Profile,VGV7519BRN))

define Profile/ARV7519RWV11
  NAME:=Livebox Astoria ARV7519 v1.1
  PACKAGES:=kmod-ltq-deu-vr9 kmod-ltq-hcd-vr9 \
	kmod-ltq-vdsl-vr9-mei kmod-ltq-vdsl-vr9 \
	kmod-ltq-atm-vr9 ltq-vdsl-vr9-fw-installer \
	ltq-vdsl-app ppp-mod-pppoa
endef

$(eval $(call Profile,ARV7519RWV11))

define Profile/ARV7519RWV12
  NAME:=Livebox Astoria ARV7519 v1.2
  PACKAGES:=kmod-ltq-deu-vr9 kmod-ltq-hcd-vr9 \
	kmod-ltq-vdsl-vr9-mei kmod-ltq-vdsl-vr9 \
	kmod-ltq-atm-vr9 ltq-vdsl-vr9-fw-installer \
	ltq-vdsl-app ppp-mod-pppoa
endef

$(eval $(call Profile,ARV7519RWV12))
