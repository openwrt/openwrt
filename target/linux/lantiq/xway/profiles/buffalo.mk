define Profile/WBMRA
  NAME:=Buffalo WBMR-HP-G300H (A) - WBMR
  PACKAGES:=kmod-ltq-hcd-ar9 kmod-ledtrig-usbdev \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-a kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath9k wpad hostapd-utils \
	swconfig
endef

$(eval $(call Profile,WBMRA))

define Profile/WBMRB
  NAME:=Buffalo WBMR-HP-G300H (B) - WBMR
  PACKAGES:=kmod-ltq-hcd-ar9 kmod-ledtrig-usbdev \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath9k wpad hostapd-utils \
	swconfig
endef

$(eval $(call Profile,WBMRB))
