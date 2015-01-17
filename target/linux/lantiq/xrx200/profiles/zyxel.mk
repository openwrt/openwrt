define Profile/P2812HNUF1
  NAME:=ZyXEL P-2812HNU-F1
  PACKAGES:=kmod-rt2800-pci wpad-mini \
	kmod-ltq-deu-vr9 kmod-ltq-hcd-vr9 \
	kmod-ltq-vdsl-vr9-mei kmod-ltq-vdsl-vr9 \
	kmod-ltq-atm-vr9 ltq-vdsl-vr9-fw-installer \
	ltq-vdsl-app ppp-mod-pppoa \
	swconfig
endef

P2812HNUF1_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
P2812HNUF1_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,P2812HNUF1))

define Profile/P2812HNUF3
  NAME:=ZyXEL P-2812HNU-F3
  PACKAGES:=kmod-rt2800-pci wpad-mini \
	kmod-ltq-deu-vr9 kmod-ltq-hcd-vr9 \
	kmod-ltq-vdsl-vr9-mei kmod-ltq-vdsl-vr9 \
	kmod-ltq-atm-vr9 ltq-vdsl-vr9-fw-installer \
	ltq-vdsl-app ppp-mod-pppoa \
	swconfig
endef

P2812HNUF3_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
P2812HNUF3_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,P2812HNUF3))

