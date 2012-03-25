define Profile/FRITZ3370
  NAME:=FRITZ3370 - Ftizbox
  PACKAGES:=kmod-leds-gpio button-hotplug kmod-usb-ifxhcd
endef

$(eval $(call Profile,FRITZ3370))
