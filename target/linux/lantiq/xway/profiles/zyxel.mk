define Profile/P2601HNFX
  NAME:=ZyXEL P-2601HN-Fx
  PACKAGES:= kmod-rt2800-usb wpad-mini \
	kmod-ltq-adsl-ar9-mei kmod-ltq-adsl-ar9 \
	kmod-ltq-adsl-ar9-fw-b kmod-ltq-atm-ar9 \
	ltq-adsl-app ppp-mod-pppoe \
	kmod-ltq-deu-ar9 kmod-ltq-hcd-ar9 \
	swconfig
endef
$(eval $(call Profile,P2601HNFX))
