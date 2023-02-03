include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=nat46

PKG_MIRROR_HASH:=09b93f31d10030d3b4f326066b544b70b1f60236d0482f27c384ed93b298c0a6
PKG_SOURCE_URL:=https://github.com/ayourtch/nat46.git
PKG_SOURCE_DATE:=2022-09-19
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=4c5beee236841724219598fabb1edc93d4f08ce5

PKG_MAINTAINER:=Hans Dedecker <dedeckeh@gmail.com>
PKG_LICENSE:=GPL-2.0

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/nat46
  DEPENDS:=@IPV6 +kmod-nf-conntrack6
  TITLE:=Stateless NAT46 translation kernel module
  SECTION:=kernel
  SUBMENU:=Network Support
  FILES:=$(PKG_BUILD_DIR)/nat46/modules/nat46.ko
  AUTOLOAD:=$(call AutoLoad,33,nat46)
endef

include $(INCLUDE_DIR)/kernel-defaults.mk

define Build/InstallDev
	mkdir -p -m 0777 $(STAGING_DIR)/usr/include/nat46
	$(CP) $(PKG_BUILD_DIR)/nat46/modules/*.h  $(STAGING_DIR)/usr/include/nat46/
endef

define Build/Compile
	+$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)/nat46/modules" \
		MODFLAGS="-DMODULE -mlong-calls" \
		EXTRA_CFLAGS="-DNAT46_VERSION=\\\"$(PKG_SOURCE_VERSION)\\\"" \
		modules
	cp $(PKG_BUILD_DIR)/nat46/modules/Module.symvers $(PKG_BUILD_DIR)/Module.symvers
endef

$(eval $(call KernelPackage,nat46))
