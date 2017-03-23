define KernelPackage/usb-dwc3-of-simple
  TITLE:=DWC3 USB simple OF driver
  DEPENDS:=+kmod-usb-dwc3
  KCONFIG:= CONFIG_USB_DWC3_OF_SIMPLE
  FILES:= $(LINUX_DIR)/drivers/usb/dwc3/dwc3-of-simple.ko
  AUTOLOAD:=$(call AutoLoad,53,dwc3-of-simple,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-dwc3-of-simple/description
 This driver provides generic platform glue for the integrated DesignWare
 USB3 IP Core.
endef

$(eval $(call KernelPackage,usb-dwc3-of-simple))

define KernelPackage/usb-phy-qcom-dwc3
  TITLE:=DWC3 USB QCOM PHY driver
  DEPENDS:=@TARGET_ipq806x +kmod-usb-dwc3-of-simple
  KCONFIG:= CONFIG_PHY_QCOM_DWC3
  FILES:= $(LINUX_DIR)/drivers/phy/phy-qcom-dwc3.ko
  AUTOLOAD:=$(call AutoLoad,45,phy-qcom-dwc3,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-phy-qcom-dwc3/description
 This driver provides support for the integrated DesignWare
 USB3 IP Core within the QCOM SoCs.
endef

$(eval $(call KernelPackage,usb-phy-qcom-dwc3))
