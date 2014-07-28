define Profile/DGN3500
  NAME:=Netgear DGN3500
  PACKAGES:=kmod-ltq-hcd-ar9 kmod-ledtrig-usbdev \
	kmod-ath9k wpad-mini \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-a kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-deu-ar9 swconfig
endef

$(eval $(call Profile,DGN3500))

define Profile/DGN3500B
  NAME:=Netgear DGN3500B
  PACKAGES:=kmod-ltq-hcd-ar9 kmod-ledtrig-usbdev \
	kmod-ath9k wpad-mini \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-deu-ar9 swconfig
endef

$(eval $(call Profile,DGN3500B))
