define Profile/EASY80920NOR
  NAME:=Lantiq VR9 - EASY80920NOR
  PACKAGES:=kmod-ath9k wpad-mini \
	kmod-ltq-deu-vr9 kmod-ltq-hcd-vr9 \
	kmod-ltq-vdsl-vr9-mei kmod-ltq-vdsl-vr9 \
	kmod-ltq-atm-vr9 ltq-vdsl-vr9-fw-installer \
	ltq-vdsl-app ppp-mod-pppoa
endef

$(eval $(call Profile,EASY80920NOR))

define Profile/EASY80920NAND
  NAME:=Lantiq VR9 - EASY80920NAND
  PACKAGES:=kmod-ath9k wpad-mini \
	kmod-ltq-deu-vr9 kmod-ltq-hcd-vr9 \
	kmod-ltq-vdsl-vr9-mei kmod-ltq-vdsl-vr9 \
	kmod-ltq-atm-vr9 ltq-vdsl-vr9-fw-installer \
	ltq-vdsl-app ppp-mod-pppoa
endef

$(eval $(call Profile,EASY80920NAND))
