# $Id$

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME:=hostap-driver
PKG_VERSION:=0.4.7
PKG_RELEASE:=1
PKG_MD5SUM:=ee495686cf27011b4e401963c2c7f62a

PKG_SOURCE_URL:=http://hostap.epitest.fi/releases/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_HOSTAP,kmod-hostap,$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE))))
$(eval $(call PKG_template,KMOD_HOSTAP_PCI,kmod-hostap-pci,$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE))))
$(eval $(call PKG_template,KMOD_HOSTAP_PLX,kmod-hostap-plx,$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: 
	$(MAKE) -C $(LINUX_DIR)/ \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)/driver/modules" \
		modules
	touch $@

$(IPKG_KMOD_HOSTAP):
	install -m0755 -d $(IDIR_KMOD_HOSTAP)/lib/modules/$(LINUX_VERSION)
	$(CP) \
		$(PKG_BUILD_DIR)/driver/modules/hostap.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_ccmp.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_tkip.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_wep.$(LINUX_KMOD_SUFFIX) \
		$(IDIR_KMOD_HOSTAP)/lib/modules/$(LINUX_VERSION)/
	$(IPKG_BUILD) $(IDIR_KMOD_HOSTAP) $(PACKAGE_DIR)

$(IPKG_KMOD_HOSTAP_PCI):
	install -m0755 -d $(IDIR_KMOD_HOSTAP_PCI)/lib/modules/$(LINUX_VERSION)
	$(CP) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_pci.$(LINUX_KMOD_SUFFIX) \
		$(IDIR_KMOD_HOSTAP_PCI)/lib/modules/$(LINUX_VERSION)/
	$(IPKG_BUILD) $(IDIR_KMOD_HOSTAP_PCI) $(PACKAGE_DIR)

$(IPKG_KMOD_HOSTAP_PLX):
	install -m0755 -d $(IDIR_KMOD_HOSTAP_PLX)/lib/modules/$(LINUX_VERSION)
	$(CP) \
		$(PKG_BUILD_DIR)/driver/modules/hostap_plx.$(LINUX_KMOD_SUFFIX) \
		$(IDIR_KMOD_HOSTAP_PLX)/lib/modules/$(LINUX_VERSION)/
	$(IPKG_BUILD) $(IDIR_KMOD_HOSTAP_PLX) $(PACKAGE_DIR)

