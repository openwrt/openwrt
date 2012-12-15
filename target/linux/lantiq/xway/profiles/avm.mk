define Profile/FRITZ7320
  NAME:=1&1 HomeServer - FRITZ7320
  PACKAGES:=kmod-ath9k wpad-mini \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-deu-ar9
endef

$(eval $(call Profile,FRITZ7320))
