define Profile/FRITZ3370
  NAME:=Fritz!Box WLan - FRITZ3370
  PACKAGES:=kmod-ath9k wpad-mini \
	kmod-ltq-deu-vr9 kmod-ltq-hcd-vr9 \
	kmod-ltq-vdsl-vr9-mei kmod-ltq-vdsl-vr9 \
	kmod-ltq-atm-vr9 ltq-vdsl-vr9-fw-installer \
	ltq-vdsl-app ppp-mod-pppoa
endef

$(eval $(call Profile,FRITZ3370))
