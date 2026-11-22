define KernelPackage/phy-sf19a2890-usb
  TITLE:=Siflower SF19A2890 USB 2.0 PHY Driver
  KCONFIG:=CONFIG_PHY_SF19A2890_USB
  DEPENDS:=@TARGET_siflower_sf19a2890
  SUBMENU:=$(USB_MENU)
  FILES:=$(LINUX_DIR)/drivers/phy/siflower/phy-sf19a2890-usb.ko
  AUTOLOAD:=$(call AutoLoad,45,phy-sf19a2890-usb,1)
endef

define KernelPackage/phy-sf19a2890-usb/description
  Support for Siflower SF19A2890 USB 2.0 PHY connected to the USB
  controller.
endef

$(eval $(call KernelPackage,phy-sf19a2890-usb))

define KernelPackage/phy-sf21-usb
  TITLE:=Siflower SF21 USB 2.0 PHY Driver
  KCONFIG:=CONFIG_PHY_SF21_USB
  DEPENDS:=@TARGET_siflower_sf21
  SUBMENU:=$(USB_MENU)
  FILES:=$(LINUX_DIR)/drivers/phy/siflower/phy-sf21-usb.ko
  AUTOLOAD:=$(call AutoLoad,45,phy-sf21-usb,1)
endef

define KernelPackage/phy-sf21-usb/description
  Support for Siflower SF21 USB 2.0 PHY connected to the USB
  controller.
endef

$(eval $(call KernelPackage,phy-sf21-usb))

define KernelPackage/sf21-eth
  TITLE:=Siflower SF21 Ethernet MAC and XPCS drivers
  KCONFIG:= \
	CONFIG_NET_SIFLOWER_ETH_DPNS=n \
	CONFIG_NET_SIFLOWER_ETH_XGMAC \
	CONFIG_NET_SIFLOWER_ETH_XPCS
  DEPENDS:=@TARGET_siflower_sf21 +PACKAGE_kmod-sf21-eth:kmod-sf21-dpns +kmod-phylink
  SUBMENU:=$(NETWORK_DEVICES_MENU)
  FILES:= \
	$(LINUX_DIR)/drivers/net/ethernet/siflower/sfxpcs.ko \
	$(LINUX_DIR)/drivers/net/ethernet/siflower/sfxgmac.ko
  AUTOLOAD:=$(call AutoLoad,58,sfxpcs sfxgmac,1)
endef

define KernelPackage/sf21-eth/description
  Siflower SF21 Ethernet MAC (xgmac) and PCS drivers. They are loaded after
  the DPNS (NPU) driver from kmod-sf21-dpns, which must initialize the
  dataplane first. The Ethernet DMA driver stays built into the kernel.
endef

$(eval $(call KernelPackage,sf21-eth))
