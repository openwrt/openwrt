define Profile/Generic
  NAME:=Generic - all boards
  PACKAGES:=kmod-leds-gpio button-hotplug
endef

$(eval $(call Profile,Generic))
