# $Id$

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME := kmod-switch
PKG_RELEASE := 1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_SWITCH,$(PKG_NAME),$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE))))

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	touch $@

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		PATH="$(TARGET_PATH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="-DBCMGPIO2" \
		modules
	touch $@
	
$(IPKG_KMOD_SWITCH):
	install -m0755 -d $(IDIR_KMOD_SWITCH)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/*.$(LINUX_KMOD_SUFFIX) \
		$(IDIR_KMOD_SWITCH)/lib/modules/$(LINUX_VERSION)
	$(IPKG_BUILD) $(IDIR_KMOD_SWITCH) $(PACKAGE_DIR)

