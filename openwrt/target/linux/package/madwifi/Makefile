# $Id$

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME:=madwifi
PKG_VERSION:=r1329-20051121
PKG_RELEASE:=1
PKG_MD5SUM:=3441f239bc2f902230025be8d9ab926c

PKG_SOURCE_URL:=http://snapshots.madwifi.org
PKG_SOURCE:=$(PKG_NAME)-trunk-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-trunk-$(PKG_VERSION)

ifeq ($(KERNEL_DIR),)
KERNEL_DIR:=$(LINUX_DIR)
endif

ifeq ($(ARCH),mips)
HAL_TARGET:=mips-be-elf
endif
ifeq ($(ARCH),mipsel)
HAL_TARGET:=mips-le-elf
endif
ifeq ($(ARCH),i386)
HAL_TARGET:=i386-elf
endif

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_MADWIFI,kmod-madwifi,$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel-$(LINUX_VERSION)-$(BOARD) ($(KERNEL_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		PATH="$(TARGET_PATH)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		TARGET="$(HAL_TARGET)" \
		TOOLPREFIX="$(KERNEL_CROSS)" \
		KERNELPATH="$(KERNEL_DIR)" \
		all
	touch $@

$(IPKG_KMOD_MADWIFI):
	mkdir -p $(IDIR_KMOD_MADWIFI)/etc/modules.d
	mkdir -p $(IDIR_KMOD_MADWIFI)/etc/init.d
	mkdir -p $(IDIR_KMOD_MADWIFI)/lib/modules/$(LINUX_VERSION)
	install -m0644 ./files/madwifi.modules $(IDIR_KMOD_MADWIFI)/etc/modules.d/20-madwifi
	install -m0755 ./files/madwifi.init $(IDIR_KMOD_MADWIFI)/etc/init.d/S20madwifi
	cp -fpR $(PKG_BUILD_DIR)/net80211/wlan*.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/ath_hal/ath_hal.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/ath/ath_pci.$(LINUX_KMOD_SUFFIX) \
		$(IDIR_KMOD_MADWIFI)/lib/modules/$(LINUX_VERSION)/
	$(IPKG_BUILD) $(IDIR_KMOD_MADWIFI) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/include/madwifi/net80211/ieee80211.h: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include/madwifi
	cp -fpR $(PKG_BUILD_DIR)/include $(STAGING_DIR)/usr/include/madwifi/
	mkdir -p $(STAGING_DIR)/usr/include/madwifi/net80211
	cp -fpR $(PKG_BUILD_DIR)/net80211/*.h $(STAGING_DIR)/usr/include/madwifi/net80211/
	touch $@

install-dev: $(STAGING_DIR)/usr/include/madwifi/net80211/ieee80211.h

uninstall-dev:
	rm -rf $(STAGING_DIR)/usr/include/madwifi

compile-targets: install-dev
clean-targets: uninstall-dev
