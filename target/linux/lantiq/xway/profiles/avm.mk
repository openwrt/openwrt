define Profile/FRITZ7320
  NAME:=1&1 HomeServer - FRITZ7320
  PACKAGES:=kmod-ath9k wpad-mini \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-deu-ar9 kmod-ltq-hcd-ar9
endef

$(eval $(call Profile,FRITZ7320))

define Profile/FRITZ3370
  NAME:=Fritz!Box WLan - FRITZ3370
  PACKAGES:=kmod-ath9k wpad-mini \
	kmod-ltq-deu-vr9 kmod-ltq-hcd-vr9 \
	ppp-mod-pppoa
endef

$(eval $(call Profile,FRITZ3370))
