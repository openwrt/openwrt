# $Id$

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME := kmod-switch
PKG_RELEASE := 1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_SWITCH,$(PKG_NAME),$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(KERNEL_RELEASE))))

ifeq ($(KERNEL_DIR),)
KERNEL_DIR:=$(LINUX_DIR)
endif
KERNEL_VERSION=$(shell echo "$(LINUX_VERSION)" | cut -d. -f1,2)

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $(PKG_BUILD_DIR)
	cp -fpR ./src/* $(PKG_BUILD_DIR)/
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		PATH="$(TARGET_PATH)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		KERNEL_VERSION="$(KERNEL_VERSION)" \
		KERNEL_DIR="$(KERNEL_DIR)" \
		EXTRA_CFLAGS="-DBCMGPIO2" \
		all
	touch $@
	
$(IPKG_KMOD_SWITCH):
	install -m0755 $(IDIR_KMOD_SWITCH)/lib/modules/$(LINUX_VERSION)
	install -m0755 -d $(IDIR_KMOD_SWITCH)/etc/modules.d
	cp -fpR $(PKG_BUILD_DIR)/*.$(LINUX_KMOD_SUFFIX) \
		$(IDIR_KMOD_SWITCH)/lib/modules/$(LINUX_VERSION)
	echo "switch-core" > $(IDIR_KMOD_SWITCH)/etc/modules.d/05-switch
	echo "switch-robo" >> $(IDIR_KMOD_SWITCH)/etc/modules.d/05-switch
	echo "switch-adm" >> $(IDIR_KMOD_SWITCH)/etc/modules.d/05-switch
	$(IPKG_BUILD) $(IDIR_KMOD_SWITCH) $(PACKAGE_DIR)

