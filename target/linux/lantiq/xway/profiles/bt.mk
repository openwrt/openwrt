define Profile/BTHOMEHUBV2B
  NAME:=BT Home Hub 2B
  PACKAGES:=kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	kmod-ltq-deu-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ath9k wpad-mini \
	swconfig
endef

BTHOMEHUBV2B_UBIFS_OPTS:="-m 512 -e 15872 -c 1959"
BTHOMEHUBV2B_UBI_OPTS:="-m 512 -p 16KiB -s 256"


$(eval $(call Profile,BTHOMEHUBV2B))
