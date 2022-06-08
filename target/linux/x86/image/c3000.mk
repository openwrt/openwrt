define Device/AG1002X
  DEVICE_TITLE := Huachenlink AG1002X
  DEVICE_PACKAGES += kmod-bnx2 kmod-e1000e kmod-e1000 kmod-forcedeth kmod-igb \
	kmod-ixgbe kmod-ice kmod-usb-net-cdc-mbim kmod-i40e
  GRUB2_VARIANT := generic
  SUPPORTED_DEVICES := AG1002X
endef
TARGET_DEVICES += AG1002X
