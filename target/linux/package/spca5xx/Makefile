# $Id$

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME:=spca5xx
PKG_VERSION:=20051212
PKG_RELEASE:=1
PKG_MD5SUM:=f8212aa6f5e412201eefd328c3608c8d

PKG_SOURCE_URL:=http://mxhaard.free.fr/spca50x/Download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

ifeq ($(KERNEL_DIR),)
KERNEL_DIR:=$(LINUX_DIR)
endif
KERNEL_VERSION=$(shell echo "$(LINUX_VERSION)" | cut -d. -f1,2)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_SPCA5XX,kmod-spca5xx,$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(KERNEL_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: 
	$(MAKE) -C $(KERNEL_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		KERNELVERSION="$(KERNEL_VERSION)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
	touch $@

$(IPKG_KMOD_SPCA5XX):
	install -m0755 -d $(IDIR_KMOD_SPCA5XX)/lib/modules/$(LINUX_VERSION)
	cp -fpR $(PKG_BUILD_DIR)/spca5xx.$(LINUX_KMOD_SUFFIX) \
		$(IDIR_KMOD_SPCA5XX)/lib/modules/$(LINUX_VERSION)/
	$(RSTRIP_KMOD) $(IDIR_KMOD_SPCA5XX)
	$(IPKG_BUILD) $(IDIR_KMOD_SPCA5XX) $(PACKAGE_DIR)

