define KernelPackage/usb-fotg210
  TITLE:=Support for FOTG210 USB host and device controllers
  DEPENDS:=@USB_SUPPORT @TARGET_gemini
  KCONFIG:=CONFIG_USB_FOTG210 \
	CONFIG_USB_FOTG210_HCD
  FILES:=$(if $(CONFIG_USB_FOTG210_HCD),$(LINUX_DIR)/drivers/usb/host/fotg210-hcd.ko@lt6.1) \
	$(if $(CONFIG_USB_FOTG210),$(LINUX_DIR)/drivers/usb/fotg210/fotg210.ko@ge6.1)
  AUTOLOAD:=$(call AutoLoad,50, \
	$(if $(CONFIG_USB_FOTG210_HCD),fotg210-hcd@lt6.1) \
	$(if $(CONFIG_USB_FOTG210),fotg210@ge6.1),1)
  $(call AddDepends/usb)
endef

$(eval $(call KernelPackage,usb-fotg210))
