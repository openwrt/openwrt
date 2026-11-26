include $(TOPDIR)/rules.mk

PKG_NAME:=b43-next
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/aleferri/b43-next.git
PKG_SOURCE_DATE:=2026-11-20
PKG_SOURCE_VERSION:=625d9063e97624d29c8519592eb7098cfc15812a
PKG_MIRROR_HASH:=6d912c74edaa85aff2f4c296305bc01bbd949f63e2bdb34b0151640ad7919d6e

PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Ville Takio <ville+git@takio.fi>
PKG_BUILD_PARALLEL:=1

PKG_CONFIG_DEPENDS:=CONFIG_B43_NEXT_DFS

STAMP_CONFIGURED_DEPENDS := $(STAGING_DIR)/usr/include/mac80211-backport/backport/autoconf.h

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/b43-next
  SUBMENU:=Wireless Drivers
  TITLE:=Broadcom 43xx wireless support, out of tree (b43-next)
  URL:=https://github.com/aleferri/b43-next
  KCONFIG:=CONFIG_HW_RANDOM=y
  DEPENDS:=@PCI_SUPPORT +kmod-mac80211 +kmod-lib-cordic +@DRIVER_11AC_SUPPORT \
	+@KERNEL_PAGE_POOL +b43-firmware-784-ac @!TARGET_uml \
	+!(TARGET_bcm47xx||TARGET_bcm53xx):kmod-bcma \
	+!(TARGET_bcm47xx||TARGET_bcm53xx):kmod-ssb
  CONFLICTS:=kmod-b43
  FILES:=$(PKG_BUILD_DIR)/b43-next.ko
  AUTOLOAD:=$(call AutoProbe,b43-next)
  MODPARAMS.b43-next:=fwpostfix=-784
  MENU:=1
endef

define KernelPackage/b43-next/description
 b43 out of tree, on the kernel's bcma and ssb: the AC-PHY of the BCM4352
 and BCM4360 and the 802.11n work of b43-next, for the PCIe cards and SoC
 cores b43 drives.

 It loads firmware 784.2 from /lib/firmware/b43-784. The AC-PHY files
 (b43-firmware-784-ac) come with it; add the b43-firmware-784 package of
 each other PHY type on the device (-n, -ht, -lp, -g, -lcn).
endef

define KernelPackage/b43-next/config
  if PACKAGE_kmod-b43-next

	config B43_NEXT_DFS
		bool "Radar channels in AP mode on the AC-PHY (not a certified detector)"
		default y
		help
		  Lets an AP on the BCM4352/BCM4360 run on DFS channels, with the
		  short-pulse radar detection of b43-next. Tested on the RT-AC66U
		  B1 and the RT-AC68U V3; not a certified radar detector. Without
		  it, the AC-PHY stays off the radar channels.

  endif
endef

NOSTDINC_FLAGS := \
	$(KERNEL_NOSTDINC_FLAGS) \
	-I$(STAGING_DIR)/usr/include/mac80211-backport/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211-backport \
	-I$(STAGING_DIR)/usr/include/mac80211/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211 \
	-include backport/autoconf.h \
	-include backport/backport.h

# mac80211 comes from the backports, not from the kernel's .config. No SDIO
# host, as in kmod-b43: on a kernel with MMC it would need kmod-mmc.
PKG_MAKE_FLAGS := CONFIG_MAC80211=m CONFIG_MMC= B43_NEXT_KERNEL_BUS=y
ifdef CONFIG_LEDS_TRIGGERS
  PKG_MAKE_FLAGS += CONFIG_MAC80211_LEDS=y
endif
ifdef CONFIG_B43_NEXT_DFS
  PKG_MAKE_FLAGS += B43_NEXT_DFS=y CONFIG_CFG80211_CERTIFICATION_ONUS=y
endif

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) \
		$(PKG_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)" \
		NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
		modules
endef

$(eval $(call KernelPackage,b43-next))
