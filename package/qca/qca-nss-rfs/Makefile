include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qca-rfs
PKG_RELEASE:=1

PKG_SOURCE_URL:=https://source.codeaurora.org/quic/qsdk/oss/lklm/qca-rfs
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=12f459e35770bdaec2b911863059ca882d93f309
PKG_MIRROR_HASH:=a0ec8ca71f4661a4f24467aaf240f7371d66359162b9feb814a2e2678d129fec

include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-rfs
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Support
  DEPENDS:=@TARGET_ipq806x||TARGET_ipq_ipq40xx||TARGET_ipq40xx +kmod-ipt-conntrack
  TITLE:=Kernel module for QCA Receiving Flow Steering
  FILES:=$(PKG_BUILD_DIR)/qrfs.ko
  KCONFIG:=CONFIG_NF_CONNTRACK_EVENTS=y CONFIG_NF_CONNTRACK_CHAIN_EVENTS=y
  AUTOLOAD:=$(call AutoLoad,29,qrfs)
endef

define KernelPackage/qca-rfs/description
QCA-RFS is a kernel module for ESS Receive Flow Steering.
endef


define Build/Compile
	$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		$(KERNEL_MAKE_FLAGS) \
		$(PKG_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		modules
endef

ifneq ($(CONFIG_PACKAGE_kmod-qca-rfs),)
define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/qca-rfs
	$(CP) -rf $(PKG_BUILD_DIR)/rfs_dev.h $(1)/usr/include/qca-rfs
endef
endif

define KernelPackage/qca-rfs/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/qrfs.init $(1)/etc/init.d/qrfs
endef

$(eval $(call KernelPackage,qca-rfs))
