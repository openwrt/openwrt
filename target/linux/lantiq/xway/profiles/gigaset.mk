define Profile/GIGASX76X
  NAME:=Gigaset sx76x
  PACKAGES:= kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
        kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
        kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
        ltq-adsl-app ppp-mod-pppoe \
        kmod-ath5k wpad-mini \
        swconfig
endef

$(eval $(call Profile,GIGASX76X))
