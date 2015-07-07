define Profile/BTHOMEHUBV5A
  NAME:=BT Home Hub 5A
  PACKAGES:=kmod-ath9k kmod-ath10k wpad-mini kmod-usb-dwc2
endef

BTHOMEHUBV5A_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
BTHOMEHUBV5A_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,BTHOMEHUBV5A))
