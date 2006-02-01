# $Id$

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME := mini_fo
PKG_VERSION := 0.6.2pre1
PKG_RELEASE := 1
PKG_MD5SUM := acd12157be25d7b3c8df90914049403e

PKG_SOURCE_URL := http://www.denx.de/twiki/pub/Know/MiniFOHome/
PKG_SOURCE := mini_fo-0-6-2-pre1.tar.bz2
PKG_CAT := bzcat

PKG_BUILD_DIR := $(BUILD_DIR)/mini_fo-0-6-2-pre1
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_MINI_FO,kmod-mini-fo,$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: 
	$(MAKE) -C $(PKG_BUILD_DIR) \
		PATH="$(TARGET_PATH)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		KERNELVERSION="$(KERNEL)" \
		KERNEL_SRC="$(LINUX_DIR)" \
		all
	touch $@

$(IPKG_KMOD_MINI_FO):
	install -m0755 -d $(IDIR_KMOD_MINI_FO)/lib/modules/$(LINUX_VERSION)
	install -m0755 -d $(IDIR_KMOD_MINI_FO)/etc/modules.d
	$(CP) $(PKG_BUILD_DIR)/mini_fo.$(LINUX_KMOD_SUFFIX) \
	 $(IDIR_KMOD_MINI_FO)/lib/modules/$(LINUX_VERSION)/
	echo "mini_fo" > $(IDIR_KMOD_MINI_FO)/etc/modules.d/15-mini-fo
	$(IPKG_BUILD) $(IDIR_KMOD_MINI_FO) $(PACKAGE_DIR)

