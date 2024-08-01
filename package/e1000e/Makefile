include $(TOPDIR)/rules.mk

PKG_NAME:=e1000e
PKG_VERSION:=3.8.4
PKG_RELEASE:=1

PKG_SOURCE:=e1000e-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://downloadmirror.intel.com/15817/eng/
PKG_HASH:=1e44dcb77cf170a3c7c80bffcd9f7c334d1a8b2d5047762449a3e73c80d29b7a

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/e1000e
  SUBMENU:=Network Devices
  TITLE:=Intel(R) PRO/1000 PCI-Express Network Driver
  FILES:=$(PKG_BUILD_DIR)/src/e1000e.ko
  AUTOLOAD:=$(call AutoLoad,50,e1000e)
  DEPENDS:=@PCI_SUPPORT
endef

define Build/Prepare
	$(MKDIR) $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)/src" \
		modules
endef

define Package/e1000e/install
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/e1000e.ko $(1)/lib/modules/$(LINUX_VERSION)
endef

$(eval $(call KernelPackage,e1000e))

