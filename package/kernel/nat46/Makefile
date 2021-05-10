include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=nat46

PKG_MIRROR_HASH:=5cc35d11ad6937734f830be5e260eb17a7b0561e37f538e5fc99026d0f2ea517
PKG_SOURCE_URL:=https://github.com/ayourtch/nat46.git
PKG_SOURCE_DATE:=2020-08-17
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=362640b41ae52b732d9e9729e61ac555492442a3

PKG_MAINTAINER:=Hans Dedecker <dedeckeh@gmail.com>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/nat46
  DEPENDS:=@IPV6
  TITLE:=Stateless NAT46 translation kernel module
  SECTION:=kernel
  SUBMENU:=Network Support
  FILES:=$(PKG_BUILD_DIR)/nat46/modules/nat46.ko
  AUTOLOAD:=$(call AutoLoad,33,nat46)
endef

include $(INCLUDE_DIR)/kernel-defaults.mk

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)/nat46/modules" \
		MODFLAGS="-DMODULE -mlong-calls" \
		EXTRA_CFLAGS="-DNAT46_VERSION=\\\"$(PKG_SOURCE_VERSION)\\\"" \
		modules
endef

$(eval $(call KernelPackage,nat46))
