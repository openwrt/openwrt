# $Id: Makefile 1832 2005-09-03 19:27:27Z wbx $

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME:=hostap-driver
PKG_VERSION:=0.4.5
PKG_RELEASE:=1
PKG_MD5SUM:=8ee17f206e035075b20fcba96b168234

PKG_SOURCE_URL:=http://hostap.epitest.fi/releases/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

ifeq ($(KERNEL_DIR),)
KERNEL_DIR:=$(LINUX_DIR)
endif

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_HOSTAP,kmod-hostap,$(LINUX_VERSION)$(BOARD)+$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(KERNEL_RELEASE))))
$(eval $(call PKG_template,KMOD_HOSTAP_PCI,kmod-hostap-pci,$(LINUX_VERSION)$(BOARD)+$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(KERNEL_RELEASE))))
$(eval $(call PKG_template,KMOD_HOSTAP_PLX,kmod-hostap-plx,$(LINUX_VERSION)$(BOARD)+$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(KERNEL_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: 
	$(MAKE) -C $(KERNEL_DIR)/ \
		SUBDIRS="$(PKG_BUILD_DIR)/driver/modules" \
		modules
	touch $@

$(IPKG_KMOD_HOSTAP):
	install -m0755 -d $(IDIR_KMOD_HOSTAP)/lib/modules/$(LINUX_VERSION)
	cp -fpR \
		$(PKG_BUILD_DIR)/driver/modules/hostap.o \
		$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_ccmp.o \
		$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_tkip.o \
		$(PKG_BUILD_DIR)/driver/modules/hostap_crypt_wep.o \
		$(IDIR_KMOD_HOSTAP)/lib/modules/$(LINUX_VERSION)/
	$(RSTRIP_KMOD) $(IDIR_KMOD_HOSTAP)
	$(IPKG_BUILD) $(IDIR_KMOD_HOSTAP) $(PACKAGE_DIR)

$(IPKG_KMOD_HOSTAP_PCI):
	install -m0755 -d $(IDIR_KMOD_HOSTAP_PCI)/lib/modules/$(LINUX_VERSION)
	cp -fpR \
		$(PKG_BUILD_DIR)/driver/modules/hostap_pci.o \
		$(IDIR_KMOD_HOSTAP_PCI)/lib/modules/$(LINUX_VERSION)/
	$(RSTRIP_KMOD) $(IDIR_KMOD_HOSTAP_PCI)
	$(IPKG_BUILD) $(IDIR_KMOD_HOSTAP_PCI) $(PACKAGE_DIR)

$(IPKG_KMOD_HOSTAP_PLX):
	install -m0755 -d $(IDIR_KMOD_HOSTAP_PLX)/lib/modules/$(LINUX_VERSION)
	cp -fpR \
		$(PKG_BUILD_DIR)/driver/modules/hostap_plx.o \
		$(IDIR_KMOD_HOSTAP_PLX)/lib/modules/$(LINUX_VERSION)/
	$(RSTRIP_KMOD) $(IDIR_KMOD_HOSTAP_PLX)
	$(IPKG_BUILD) $(IDIR_KMOD_HOSTAP_PLX) $(PACKAGE_DIR)

