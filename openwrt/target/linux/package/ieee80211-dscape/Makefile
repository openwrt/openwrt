# $Id$

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME := kmod-ieee80211-dscape
PKG_RELEASE := 1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_IEEE80211_DSCAPE,$(PKG_NAME),$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE))))

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $(PKG_BUILD_DIR)
	cp -fpR ./src/* $(PKG_BUILD_DIR)/
	touch $@

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		PATH="$(TARGET_PATH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		LINUXINCLUDE="-I$(shell pwd)/src/include -I$(LINUX_DIR)/include" \
		modules
	touch $@
	
$(IPKG_KMOD_IEEE80211_DSCAPE):
	install -m0755 -d $(IDIR_KMOD_IEEE80211_DSCAPE)/lib/modules/$(LINUX_VERSION)
	#install -m0755 -d $(IDIR_KMOD_IEEE80211_DSCAPE)/etc/modules.d
	cp -fpR $(PKG_BUILD_DIR)/*.ko $(IDIR_KMOD_IEEE80211_DSCAPE)/lib/modules/$(LINUX_VERSION)
	$(IPKG_BUILD) $(IDIR_KMOD_IEEE80211_DSCAPE) $(PACKAGE_DIR)

install-dev:
	mkdir -p $(STAGING_DIR)/usr/include/ieee80211-dscape
	cp -fpR src/include/* $(STAGING_DIR)/usr/include/ieee80211-dscape/

uninstall-dev:
	rm -rf $(STAGING_DIR)/usr/include/ieee80211-dscape

compile-targets: install-dev
clean-targets: uninstall-dev
