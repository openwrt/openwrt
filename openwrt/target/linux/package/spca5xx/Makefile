# $Id$

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME:=spca5xx
PKG_VERSION:=20060301
PKG_RELEASE:=1
PKG_MD5SUM:=4160e03c408ac254111febacf058b88a

PKG_SOURCE_URL:=http://mxhaard.free.fr/spca50x/Download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_SPCA5XX,kmod-spca5xx,$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: 
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		KERNELVERSION="$(KERNEL)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
	touch $@

$(IPKG_KMOD_SPCA5XX):
	install -m0755 -d $(IDIR_KMOD_SPCA5XX)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/spca5xx.$(LINUX_KMOD_SUFFIX) \
		$(IDIR_KMOD_SPCA5XX)/lib/modules/$(LINUX_VERSION)/
	$(RSTRIP_KMOD) $(IDIR_KMOD_SPCA5XX)
	$(IPKG_BUILD) $(IDIR_KMOD_SPCA5XX) $(PACKAGE_DIR)

