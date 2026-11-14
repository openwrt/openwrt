include $(TOPDIR)/rules.mk

PKG_NAME:=b43-next
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/aleferri/b43-next.git
PKG_SOURCE_DATE:=2026-11-14
PKG_SOURCE_VERSION:=ee7ed443dc8dde43bd2b2e9aea7d170ad5e92e1b
PKG_MIRROR_HASH:=685036229d3ceee7c5e44827a5ccbba7abbc0c6e0a7b4085b00b1aab46092a51

PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Ville Takio <ville+git@takio.fi>
PKG_BUILD_PARALLEL:=1

PKG_CONFIG_DEPENDS:= \
	CONFIG_B43_NEXT_DFS \
	CONFIG_B43_NEXT_FW_SQUASH \
	CONFIG_B43_NEXT_FW_SQUASH_COREREVS \
	CONFIG_B43_NEXT_FW_SQUASH_PHYTYPES

STAMP_CONFIGURED_DEPENDS := $(STAGING_DIR)/usr/include/mac80211-backport/backport/autoconf.h

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

# The b43 firmware, as kmod-b43 extracts it: 784.2 from driver 6.30.163.46
B43_NEXT_FW_NAME:=broadcom-wl
B43_NEXT_FW_VERSION:=6.30.163.46
B43_NEXT_FW_OBJECT:=$(B43_NEXT_FW_NAME)-$(B43_NEXT_FW_VERSION).wl_apsta.o
B43_NEXT_FW_SOURCE:=$(B43_NEXT_FW_NAME)-$(B43_NEXT_FW_VERSION).tar.bz2

define Download/b43-next-fw
  FILE:=$(B43_NEXT_FW_SOURCE)
  URL:=@SF/b43-firmware
  HASH:=a07c3b6b277833c7dbe61daa511f908cd66c5e2763eb7a0859abc36cd9335c2d
endef
$(eval $(call Download,b43-next-fw))

define KernelPackage/b43-next
  SUBMENU:=Wireless Drivers
  TITLE:=Broadcom 43xx wireless support, out of tree (b43-next)
  URL:=https://github.com/aleferri/b43-next
  KCONFIG:=CONFIG_HW_RANDOM=y
  DEPENDS:=@PCI_SUPPORT +kmod-mac80211 +kmod-lib-cordic +@DRIVER_11AC_SUPPORT \
	+@KERNEL_PAGE_POOL @!TARGET_uml
  CONFLICTS:=kmod-b43
  FILES:=$(PKG_BUILD_DIR)/b43-next.ko
  AUTOLOAD:=$(call AutoProbe,b43-next)
  MENU:=1
endef

define KernelPackage/b43-next/description
 b43 with its own copies of the ssb and bcma buses, in one module that
 loads next to the kernel's ssb and bcma, with the AC-PHY of the BCM4352
 and BCM4360 and the 802.11n work of b43-next. It drives the PCIe cards
 b43 drives; at boot, /etc/init.d/b43-next hands them from the kernel's
 bcma or ssb PCI bridge to b43-next's and moves their radios' paths in
 /etc/config/wireless along. The 802.11 cores built into a SoC stay with
 kmod-b43.
endef

define KernelPackage/b43-next/config
  if PACKAGE_kmod-b43-next

	config B43_NEXT_DFS
		bool "Radar channels in AP mode on the AC-PHY (not a certified detector)"
		default n
		help
		  Lets an AP on the BCM4352/BCM4360 run on DFS channels, with the
		  short-pulse radar detection of b43-next. Tested on the RT-AC66U
		  B1 only; not a certified radar detector.

	config B43_NEXT_FW_SQUASH
		bool "Remove unnecessary firmware files"
		default n
		help
		  Keeps only the firmware files of the selected PHY types and
		  core revisions.

	config B43_NEXT_FW_SQUASH_COREREVS
		string "Core revisions to include"
		depends on B43_NEXT_FW_SQUASH
		default "30,42"
		help
		  Comma separated core revisions, e.g. 30 for the BCM43217 and
		  BCM43227, 42 for the BCM4352 and BCM4360.

	config B43_NEXT_FW_SQUASH_PHYTYPES
		string "PHY types to include"
		depends on B43_NEXT_FW_SQUASH
		default "N,AC"
		help
		  Comma separated PHY types: A, G, N, LP, HT, AC.

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
PKG_MAKE_FLAGS := CONFIG_MAC80211=m CONFIG_MMC=
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
	modinfo -F alias $(PKG_BUILD_DIR)/b43-next.ko | grep '^pci:' | sort -u \
		> $(PKG_BUILD_DIR)/pci-aliases
	tar xjf "$(DL_DIR)/$(B43_NEXT_FW_SOURCE)" -C "$(PKG_BUILD_DIR)"
endef

define KernelPackage/b43-next/install
	$(INSTALL_DIR) $(1)/lib/firmware
	b43-fwcutter -w $(1)/lib/firmware/ $(PKG_BUILD_DIR)/$(B43_NEXT_FW_OBJECT)
ifneq ($(CONFIG_B43_NEXT_FW_SQUASH),)
	b43-fwsquash.py "$(CONFIG_B43_NEXT_FW_SQUASH_PHYTYPES)" \
		"$(CONFIG_B43_NEXT_FW_SQUASH_COREREVS)" "$(1)/lib/firmware/b43"
endif
	$(INSTALL_DIR) $(1)/usr/share/b43-next
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/pci-aliases $(1)/usr/share/b43-next/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/b43-next.init $(1)/etc/init.d/b43-next
endef

$(eval $(call KernelPackage,b43-next))
