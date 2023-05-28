define KernelPackage/pcie-starfive
  SUBMENU:=$(OTHER_MENU)
  TITLE:=JH7110 PCIe controller support
  DEPENDS:=@TARGET_starfive
  KCONFIG:=CONFIG_PCIE_STARFIVE
  FILES:=$(LINUX_DIR)/drivers/pci/controller/pcie-starfive.ko
  AUTOLOAD:=$(call AutoLoad,41,pcie-starfive,1)
endef

define KernelPackage/pcie-starfive/description
 PCIe support for JH7110
endef

$(eval $(call KernelPackage,pcie-starfive))


define KernelPackage/usb-cdns3-starfive
  TITLE:=Cadence USB3 StarFive USB driver
  DEPENDS:=@TARGET_starfive +kmod-usb-cdns3
  KCONFIG:= CONFIG_USB_CDNS3_STARFIVE
  FILES:= $(LINUX_DIR)/drivers/usb/cdns3/cdns3-starfive.ko
  AUTOLOAD:=$(call AutoLoad,53,cdns3-starfive,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-cdns3-starfive/description
 Add support for the Cadence USB3 controller found in StarFive SoCs.
endef


$(eval $(call KernelPackage,usb-cdns3-starfive))
