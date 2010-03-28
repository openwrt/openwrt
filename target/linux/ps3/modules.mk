define KernelPackage/ps3lan
  SUBMENU:=Network Devices
  TITLE:=PS3-Lan support
  DEPENDS:=@LINUX_2_6 @TARGET_ps3
  KCONFIG:= \
	CONFIG_GELIC_NET \
	CONFIG_GELIC_WIRELESS=y
  FILES:=$(LINUX_DIR)/drivers/net/ps3_gelic.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,ps3_gelic)
endef

define KernelPackage/ps3lan/description
  Kernel module for PS3 Networking. Includes Gb Ethernet and WLAN
endef

$(eval $(call KernelPackage,ps3lan))
