# $Id$

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME := mini_fo
PKG_VERSION := 0.6.1-pre1
PKG_RELEASE := 1
PKG_MD5SUM := 7e192e01b08023c7f3e63bf1c796bc14

PKG_SOURCE_URL := http://www.denx.de/twiki/pub/Know/MiniFOHome/
PKG_SOURCE := mini_fo-0-6-1-pre1.tar.bz2
PKG_CAT := bzcat

PKG_BUILD_DIR := $(BUILD_DIR)/mini_fo
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

ifeq ($(KERNEL_DIR),)
KERNEL_DIR:=$(LINUX_DIR)
endif
KERNEL_VERSION=$(shell echo "$(LINUX_VERSION)" | cut -d. -f1,2)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_MINI_FO,kmod-mini-fo,$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(KERNEL_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: 
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		KERNELVERSION="$(KERNEL_VERSION)" \
		KERNEL_SRC="$(KERNEL_DIR)" \
		all
	touch $@

$(IPKG_KMOD_MINI_FO):
	install -m0755 -d $(IDIR_KMOD_MINI_FO)/lib/modules/$(LINUX_VERSION)
	install -m0755 -d $(IDIR_KMOD_MINI_FO)/etc/modules.d
	cp -fpR $(PKG_BUILD_DIR)/mini_fo.o \
	 $(IDIR_KMOD_MINI_FO)/lib/modules/$(LINUX_VERSION)/
	echo "mini_fo" > $(IDIR_KMOD_MINI_FO)/etc/modules.d/15-mini-fo
	$(RSTRIP_KMOD) $(IDIR_KMOD_MINI_FO)
	$(IPKG_BUILD) $(IDIR_KMOD_MINI_FO) $(PACKAGE_DIR)

