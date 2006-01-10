# $Id$

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME := bcm43xx-dscape
PKG_VERSION := 20060104
PKG_RELEASE := 1

PKG_SOURCE_URL := http://ftp.berlios.de/pub/bcm43xx/snapshots/bcm43xx
PKG_SOURCE := $(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT := bzcat

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

FIRMWARE_BLOB := wl_apsta-3.130.20.0.o
FIRMWARE_BLOB_URL := http://downloads.openwrt.org/sources
FIRMWARE_BLOB_MD5SUM := e08665c5c5b66beb9c3b2dd54aa80cb3

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_BCM43XX_DSCAPE,kmod-$(PKG_NAME),$(LINUX_VERSION)-$(BOARD)+0.$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/fwcutter/.built:
	mkdir -p $(PKG_BUILD_DIR)/fwcutter
	cp ./fwcutter/* $(PKG_BUILD_DIR)/fwcutter/
	$(SCRIPT_DIR)/download.pl "$(PKG_BUILD_DIR)/fwcutter" "$(FIRMWARE_BLOB)" "$(FIRMWARE_BLOB_MD5SUM)" $(FIRMWARE_BLOB_URL)
	$(MAKE) -C $(PKG_BUILD_DIR)/fwcutter
	$(PKG_BUILD_DIR)/fwcutter/fwcutter -w $(PKG_BUILD_DIR)/fwcutter $(PKG_BUILD_DIR)/fwcutter/$(FIRMWARE_BLOB)
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/fwcutter/.built
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		PATH="$(TARGET_PATH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		LINUXINCLUDE="-I$(STAGING_DIR)/usr/include/ieee80211-dscape -I$(LINUX_DIR)/include -DBCM47XX=1" \
		modules
	touch $@
	
$(IPKG_KMOD_BCM43XX_DSCAPE):
	install -m0755 -d $(IDIR_KMOD_BCM43XX_DSCAPE)/lib/modules/$(LINUX_VERSION)
	install -m0755 -d $(IDIR_KMOD_BCM43XX_DSCAPE)/etc/modules.d
	( \
		echo 80211; \
		echo rate_control; \
		echo bcm43xx; \
	) >& $(IDIR_KMOD_BCM43XX_DSCAPE)/etc/modules.d/50-bcm43xx
	cp -fpR $(PKG_BUILD_DIR)/*.ko $(IDIR_KMOD_BCM43XX_DSCAPE)/lib/modules/$(LINUX_VERSION)
	install -m0755 -d $(IDIR_KMOD_BCM43XX_DSCAPE)/lib/firmware
	cp -fpR $(PKG_BUILD_DIR)/fwcutter/*.fw $(IDIR_KMOD_BCM43XX_DSCAPE)/lib/firmware/
	$(IPKG_BUILD) $(IDIR_KMOD_BCM43XX_DSCAPE) $(PACKAGE_DIR)
