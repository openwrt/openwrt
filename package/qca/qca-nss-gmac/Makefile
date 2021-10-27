include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qca-nss-gmac
PKG_RELEASE:=1

PKG_SOURCE_URL:=https://source.codeaurora.org/quic/qsdk/oss/lklm/nss-gmac
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=a550810922de3be4bbc1234d9945d5f5eeed2ea9
PKG_MIRROR_HASH:=bdea3c94a3f1f72abb9fb610595bec751b8200b83dbd93ad20f52bcbb195fcc6

include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-nss-gmac
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_ipq806x||TARGET_ipq_ipq806x @!LINUX_3_18
  TITLE:=Kernel driver for NSS gmac
  FILES:=$(PKG_BUILD_DIR)/ipq806x/qca-nss-gmac.ko
  AUTOLOAD:=$(call AutoLoad,31,qca-nss-gmac)
endef

define KernelPackage/qca-nss-gmac/description
This package contains a NSS driver for QCA chipset
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include/qca-nss-gmac
	$(CP) $(PKG_BUILD_DIR)/ipq806x/exports/* $(1)/usr/include/qca-nss-gmac/
endef

EXTRA_CFLAGS+= \
	-DCONFIG_NSS_DEBUG_LEVEL=4 \
	-I$(PKG_BUILD_DIR)/nss_hal/include \
	-I$(PKG_BUILD_DIR)/nss_hal/$(BOARD)

define Build/Compile
	$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		$(KERNEL_MAKE_FLAGS) \
		$(PKG_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,qca-nss-gmac))
