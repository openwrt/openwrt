include $(TOPDIR)/rules.mk

PKG_NAME:=r8127
PKG_VERSION:=11.017.02
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://github.com/openwrt/rtl8127/releases/download/$(PKG_VERSION)
PKG_HASH:=8be2612d9b30e9a00c33e44c71e1bf07703a0b34051be6fa6cccfaea5532b78e

PKG_BUILD_PARALLEL:=1
PKG_LICENSE:=GPLv2
PKG_MAINTAINER:=Alvaro Fernandez Rojas <noltari@gmail.com>

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/r8127
  SUBMENU:=Network Devices
  TITLE:=Realtek RTL8127 PCI 10 Gigabit Ethernet driver
  DEPENDS:=@PCI_SUPPORT +kmod-libphy
  FILES:=$(PKG_BUILD_DIR)/src/r8127.ko
  AUTOLOAD:=$(call AutoProbe,r8127,1)
  PROVIDES:=@kmod-r8169-any
  VARIANT:=regular
endef

define KernelPackage/r8127-rss
$(call KernelPackage/r8127)
  CONFLICTS:=kmod-r8127
  TITLE+= (RSS)
  VARIANT:=rss
endef

ifeq ($(BUILD_VARIANT),rss)
  PKG_MAKE_FLAGS += \
    ENABLE_MULTIPLE_TX_QUEUE=y \
    ENABLE_RSS_SUPPORT=y
endif

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) \
		$(PKG_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)/src" \
		modules
endef

$(eval $(call KernelPackage,r8127))
$(eval $(call KernelPackage,r8127-rss))
