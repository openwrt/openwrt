ARCH:=aarch64
BOARDNAME:=Generic EFI boot
FEATURES += pci pcie usb ubifs
DEVICE_PACKAGES += kmod-amazon-ena kmod-e1000e kmod-vmxnet3 kmod-rtc-rx8025
define Target/Description
endef

