# $Id: Makefile 3297 2006-03-02 23:42:52Z florian $

include $(TOPDIR)/rules.mk
include ../../rules.mk

PKG_NAME:=zd1211
PKG_VERSION:=r69
PKG_RELEASE:=1
PKG_MD5SUM:=c14133a3d5b27927402a492e84d2467a

PKG_SOURCE_URL:=http://zd1211.ath.cx/download/
PKG_SOURCE:=$(PKG_NAME)-driver-$(PKG_VERSION).tgz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-driver-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_ZD1211,kmod-$(PKG_NAME),$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE),$(ARCH),kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE))))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: 
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		CC="$(TARGET_CC)" \
		CPP="$(TARGET_CC)" \
		LD="$(TARGET_CROSS)ld" \
		STAGING_DIR=$(STAGING_DIR) \
		KERNELVERSION="$(KERNEL)" \
		MODPATH="$(MODULES_DIR)" \
		KERNEL_SOURCE="$(LINUX_DIR)" \
		all
	$(TARGET_CC) $(PKG_BUILD_DIR)/apdbg.c -o $(PKG_BUILD_DIR)/apdbg
	touch $@

$(IPKG_KMOD_ZD1211):
	install -m0755 -d $(IDIR_KMOD_ZD1211)/lib/modules/$(LINUX_VERSION) \
			  $(IDIR_KMOD_ZD1211)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/zd1211.$(LINUX_KMOD_SUFFIX) \
		$(IDIR_KMOD_ZD1211)/lib/modules/$(LINUX_VERSION)/
	$(CP) $(PKG_BUILD_DIR)/apdbg $(IDIR_KMOD_ZD1211)/usr/sbin/
	$(CP) $(PKG_BUILD_DIR)/sta $(IDIR_KMOD_ZD1211)/usr/sbin/$(PKG_NAME)-sta
	$(RSTRIP_KMOD) $(IDIR_KMOD_ZD1211)
	$(IPKG_BUILD) $(IDIR_KMOD_ZD1211) $(PACKAGE_DIR)

