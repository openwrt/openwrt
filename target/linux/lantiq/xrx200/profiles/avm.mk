define Profile/FRITZ3370
  NAME:=Fritz!Box WLan - FRITZ3370
  PACKAGES:=kmod-ath9k wpad-mini \
	kmod-ltq-deu-vr9 kmod-ltq-hcd-vr9 \
	kmod-ltq-vdsl-vr9-mei kmod-ltq-vdsl-vr9 \
	kmod-ltq-atm-vr9 ltq-vdsl-vr9-fw-installer \
	ltq-vdsl-app ppp-mod-pppoa
endef

FRITZ3370_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
FRITZ3370_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,FRITZ3370))
