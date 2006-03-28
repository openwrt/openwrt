# $Id: Makefile 3287 2006-02-28 20:09:56Z nbd $

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME:=madwifi-old
PKG_VERSION:=r1417-20060128
PKG_RELEASE:=1
PKG_MD5SUM:=95d457e0edfb7028e09dee60db7fa7b8

PKG_SOURCE_URL:=http://snapshots.madwifi.org/$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

ifeq ($(ARCH),mips)
HAL_TARGET:=mips-be-elf
endif
ifeq ($(ARCH),mipsel)
HAL_TARGET:=mips-le-elf
endif
ifeq ($(ARCH),i386)
HAL_TARGET:=i386-elf
endif

BUS := PCI

ifneq ($(BR2_LINUX_2_4_AR531X),)
	BUS := AHB
endif
# ifneq ($(BR2_LINUX_2_6_ARUBA),)
#      BUS := PCI AHB  # no suitable HAL for AHB yet.
# endif

BUS_MODULES:=
ifeq ($(findstring AHB,$(BUS)),AHB)
BUS_MODULES+=$(PKG_BUILD_DIR)/ath/ath_ahb.$(LINUX_KMOD_SUFFIX)
endif
ifeq ($(findstring PCI,$(BUS)),PCI)
BUS_MODULES+=$(PKG_BUILD_DIR)/ath/ath_pci.$(LINUX_KMOD_SUFFIX)
endif


include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_MADWIFI_OLD,kmod-madwifi-old,$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	touch $@

MADWIFI_OLD_MAKEOPTS= -C $(PKG_BUILD_DIR) \
		PATH="$(TARGET_PATH)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		TARGET="$(HAL_TARGET)" \
		TOOLPREFIX="$(KERNEL_CROSS)" \
		TOOLPATH="$(KERNEL_CROSS)" \
		KERNELPATH="$(LINUX_DIR)" \
		DOMULTI=1


$(PKG_BUILD_DIR)/.built:
ifeq ($(findstring AHB,$(BUS)),AHB)
	$(MAKE) $(MADWIFI_OLD_MAKEOPTS) BUS="AHB" all
endif
ifeq ($(findstring PCI,$(BUS)),PCI)
	$(MAKE) $(MADWIFI_OLD_MAKEOPTS) BUS="PCI" all
endif
	$(MAKE) $(MADWIFI_OLD_MAKEOPTS) -C tools all
	$(RSTRIP) $(PKG_BUILD_DIR)/tools/madwifi_multi
	touch $@

$(IPKG_KMOD_MADWIFI_OLD):
	install -d -m0755 $(IDIR_KMOD_MADWIFI_OLD)/etc/modules.d
	install -m0644 ./files/madwifi.modules $(IDIR_KMOD_MADWIFI_OLD)/etc/modules.d/20-madwifi
	install -d -m0755 $(IDIR_KMOD_MADWIFI_OLD)/etc/init.d
	install -m0755 ./files/madwifi.init $(IDIR_KMOD_MADWIFI_OLD)/etc/init.d/S20madwifi
	install -d -m0755 $(IDIR_KMOD_MADWIFI_OLD)/lib/modules/$(LINUX_VERSION)
	$(CP)   $(PKG_BUILD_DIR)/net80211/wlan*.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/ath_hal/ath_hal.$(LINUX_KMOD_SUFFIX) \
		$(BUS_MODULES) \
		$(PKG_BUILD_DIR)/ath_rate/sample/ath_rate_sample.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/net80211/*.$(LINUX_KMOD_SUFFIX) \
		$(IDIR_KMOD_MADWIFI_OLD)/lib/modules/$(LINUX_VERSION)/
	install -d -m0755 $(IDIR_KMOD_MADWIFI_OLD)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/tools/{80211debug,80211stats,athchans,athctrl,athdebug,athkey,athstats} $(IDIR_KMOD_MADWIFI_OLD)/usr/sbin/
	$(IPKG_BUILD) $(IDIR_KMOD_MADWIFI_OLD) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/include/madwifi-old/net80211/ieee80211.h: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include/madwifi-old
	$(CP) $(PKG_BUILD_DIR)/include $(STAGING_DIR)/usr/include/madwifi-old
	mkdir -p $(STAGING_DIR)/usr/include/madwifi-old/net80211
	$(CP) $(PKG_BUILD_DIR)/net80211/*.h $(STAGING_DIR)/usr/include/madwifi-old/net80211/
	touch $@

install-dev: $(STAGING_DIR)/usr/include/madwifi-old/net80211/ieee80211.h

uninstall-dev:
	rm -rf $(STAGING_DIR)/usr/include/madwifi-old

compile-targets: install-dev
clean-targets: uninstall-dev
