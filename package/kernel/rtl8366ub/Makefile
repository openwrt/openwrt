include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rtl8366ub
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/rtl8366ub
  SUBMENU:=Network Devices
  TITLE:=Realtek RTL8366UB Switch Driver
  DEPENDS:=@TARGET_mediatek +kmod-swconfig
  FILES:= \
	$(PKG_BUILD_DIR)/rtl8366ub.ko
  AUTOLOAD:=$(call AutoLoad,20,rtl8366ub)
endef

define KernelPackage/rtl8366ub/description
 Realtek RTL8366UB Switch SDK and swconfig platform driver (single module).
endef

EXTRA_CFLAGS += \
  -DCONFIG_DAL_RTL8371C \
  -DMDC_MDIO_OPERATION \
  -DCONFIG_COMPATIBLE_V1_4 \
  -DDRIVER_VERSION="\\\"$(PKG_VERSION)\\\"" \
  -I$(PKG_BUILD_DIR) \
  -I$(PKG_BUILD_DIR)/dal \
  -I$(PKG_BUILD_DIR)/dal/rtl8371c

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" EXTRA_CFLAGS="$(EXTRA_CFLAGS)" modules
endef

define KernelPackage/rtl8366ub/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./files/usr/sbin/rtl8366ub-diag.sh $(1)/usr/sbin/
endef

$(eval $(call KernelPackage,rtl8366ub))
