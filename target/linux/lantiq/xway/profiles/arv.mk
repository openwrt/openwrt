define Profile/ARV4525PW
  NAME:=Speedport W502V Typ A - ARV4525PW
  PACKAGES:=kmod-ath5k wpad-mini \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa
endef

$(eval $(call Profile,ARV4525PW))

define Profile/ARV7525PW
  NAME:=Speedport W303V Typ A - ARV7525PW
  PACKAGES:=kmod-rt2800-pci wpad-mini \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa
endef

$(eval $(call Profile,ARV7525PW))

define Profile/ARV4518PWR01
  NAME:=ARV4518PWR01
  PACKAGES:=kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
  	kmod-ath5k wpad-mini \
	swconfig
endef

$(eval $(call Profile,ARV4518PWR01))

define Profile/ARV4518PWR01A
  NAME:=ARV4518PWR01A
  PACKAGES:=kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
        kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
        kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
        ltq-adsl-app ppp-mod-pppoa \
        kmod-ath5k wpad-mini \
        swconfig
endef

$(eval $(call Profile,ARV4518PWR01A))

define Profile/ARV4510PW
  NAME:=Wippies, Elisa - ARV4510PW
  PACKAGES:=kmod-ledtrig-usbdev kmod-usb2-pci kmod-usb-uhci \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-tapi kmod-ltq-vmmc \
	kmod-rt2800-pci kmod-ath5k wpad-mini \
	swconfig
endef

$(eval $(call Profile,ARV4510PW))

define Profile/ARV4519PW
  NAME:=Vodafone, Pirelli - ARV4519PW
  PACKAGES:=kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	swconfig
endef

$(eval $(call Profile,ARV4519PW))

define Profile/ARV7510PW22
  NAME:=Astoria - ARV7510PW22
  PACKAGES:=kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-tapi kmod-ltq-vmmc \
	kmod-rt2800-pci wpad-mini \
	kmod-usb-uhci kmod-usb2 kmod-usb2-pci \
	swconfig
endef

$(eval $(call Profile,ARV7510PW22))

define Profile/ARV7518PW
  NAME:=Astoria - ARV7518PW
  PACKAGES:=kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
  	kmod-ath9k wpad-mini \
	swconfig
endef

$(eval $(call Profile,ARV7518PW))

define Profile/ARV7519PW
  NAME:=Astoria - ARV7519PW
  PACKAGES:=kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-a kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
  	kmod-rt2800-pci wpad-mini \
	swconfig
endef

$(eval $(call Profile,ARV7519PW))

define Profile/ARV4520PW
  NAME:=Easybox 800, WAV-281 - ARV4520PW
  PACKAGES:=kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
  	kmod-rt61-pci wpad-mini \
	swconfig
endef

$(eval $(call Profile,ARV4520PW))

define Profile/ARV452CQW
  NAME:=Easybox 801 - ARV452CQW
  PACKAGES:=kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
  	kmod-ath5k wpad-mini \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	swconfig
endef

$(eval $(call Profile,ARV452CQW))

define Profile/ARV752DPW
  NAME:=Easybox 802 - ARV752DPW
  PACKAGES:=kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-tapi kmod-ltq-vmmc \
	kmod-rt2800-pci wpad-mini \
	swconfig
endef

$(eval $(call Profile,ARV752DPW))

define Profile/ARV752DPW22
  NAME:=Easybox 803 - ARV752DPW22
  PACKAGES:=kmod-usb2-pci kmod-usb-uhci kmod-ltq-hcd-danube kmod-ledtrig-usbdev \
	kmod-ltq-adsl-danube-mei kmod-ltq-adsl-danube \
	kmod-ltq-adsl-danube-fw-b kmod-ltq-atm-danube \
	ltq-adsl-app ppp-mod-pppoa \
	kmod-ltq-tapi kmod-ltq-vmmc \
	kmod-rt2800-pci wpad-mini \
	swconfig
endef

$(eval $(call Profile,ARV752DPW22))
