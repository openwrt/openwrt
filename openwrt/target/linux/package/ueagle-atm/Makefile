# $Id: Makefile 2767 2005-12-25 02:10:14Z wbx $

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME:=ueagle-atm
PKG_VERSION:=1.0d1
PKG_RELEASE:=1
PKG_MD5SUM:=5da1fceb15168f4c7ad407a3e38ddecb

PKG_SOURCE_URL:=http://castet.matthieu.free.fr/eagle/release/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_UEAGLE_ATM,kmod-ueagle-atm,$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel-$(LINUX_VERSION)-$(BOARD) ($(LINUX_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		TARGET="$(HAL_TARGET)" \
		KERNELDIR="$(LINUX_DIR)" \
		all
	touch $@

$(IPKG_KMOD_UEAGLE_ATM):
	install -m0755 -d $(IDIR_KMOD_UEAGLE_ATM)/lib/modules/$(LINUX_VERSION) $(IDIR_KMOD_UEAGLE_ATM)/etc/modules.d
	install -m0644 ./files/$(PKG_NAME).modules $(IDIR_KMOD_UEAGLE_ATM)/etc/modules.d/10-$(PKG_NAME)
	$(CP)	$(PKG_BUILD_DIR)/driver/ueagle-atm.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/usbatm_iso/usbatm.$(LINUX_KMOD_SUFFIX) \
		$(IDIR_KMOD_UEAGLE_ATM)/lib/modules/$(LINUX_VERSION)/
	$(RSTRIP_KMOD) $(IDIR_KMOD_UEAGLE_ATM)
	$(IPKG_BUILD) $(IDIR_KMOD_UEAGLE_ATM) $(PACKAGE_DIR)

