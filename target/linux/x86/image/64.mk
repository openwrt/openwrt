define Device/generic
  DEVICE_TITLE := Generic x86/64
  DEVICE_PACKAGES += kmod-bnx2 kmod-e1000e kmod-e1000 kmod-forcedeth kmod-igb \
	kmod-r8169
  GRUB2_VARIANT := generic
endef
TARGET_DEVICES += generic

define Device/pcengines_apu2
  DEVICE_TITLE := PC Engines APU 2
  DEVICE_PACKAGES += amd64-microcode flashrom fstrim irqbalance kmod-bnx2 \
	kmod-crypto-hw-ccp kmod-crypto-hw-ccp kmod-e1000 kmod-e1000e \
	kmod-forcedeth kmod-gpio kmod-gpio-button-hotplug kmod-gpio-nct5104d \
	kmod-igb kmod-leds-apu2 kmod-leds-gpio kmod-pcspkr kmod-r8169 \
	kmod-sound-core kmod-sp5100_tco kmod-usb-core kmod-usb-ohci kmod-usb2 \
	kmod-usb3
  GRUB2_VARIANT := generic
  SUPPORTED_DEVICES += pcengines,apu2
endef
TARGET_DEVICES += pcengines_apu2
