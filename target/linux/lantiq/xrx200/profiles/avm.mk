define Profile/FRITZ3370
  NAME:=Fritz!Box WLan - FRITZ3370
  PACKAGES:=kmod-ath9k wpad-mini kmod-usb-dwc2
endef

FRITZ3370_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
FRITZ3370_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,FRITZ3370))
