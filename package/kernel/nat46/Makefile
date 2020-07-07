include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=nat46

PKG_MIRROR_HASH:=0627c7122ff7432aadb443e92e11a9ad7710add0ff512eebe17d7e3c041e0d2a
PKG_SOURCE_URL:=https://github.com/ayourtch/nat46.git
PKG_SOURCE_DATE:=2020-06-26
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=1182f30785e4274913f01a8c3d7e1b5437ae3819

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
