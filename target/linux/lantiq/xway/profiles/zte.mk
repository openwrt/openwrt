define Profile/H201L
  NAME:=ZTE H201L - H201L
  PACKAGES:=kmod-ath9k-htc wpad-mini \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoe \
	kmod-ltq-deu-ar9 kmod-ltq-hcd-ar9 \
	kmod-ltq-tapi kmod-ltq-vmmc \
	swconfig
endef

$(eval $(call Profile,H201L))
