# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=arptables
PKG_VERSION:=0.0.3
PKG_RELEASE:=1
PKG_MD5SUM:=1672244603c8979577aa4738be35a759
PKG_SOURCE_URL:=@SF/ebtables
PKG_SOURCE:=$(PKG_NAME)-v$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-v$(PKG_VERSION)
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,ARPTABLES,arptables,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: 
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld \
		COPT_FLAGS="$(TARGET_CFLAGS)" \
		KERNEL_DIR="./include/linux"
	touch $@

$(IPKG_ARPTABLES):
	install -d -m0755 $(IDIR_ARPTABLES)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/arptables $(IDIR_ARPTABLES)/usr/sbin/
	$(RSTRIP) $(IDIR_ARPTABLES)
	$(IPKG_BUILD) $(IDIR_ARPTABLES) $(PACKAGE_DIR)
