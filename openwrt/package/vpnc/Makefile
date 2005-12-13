include $(TOPDIR)/rules.mk

PKG_NAME:=vpnc
PKG_VERSION:=0.3.3
PKG_RELEASE:=1

PKG_MD5SUM:=e7518cff21326fe7eb9795b60c25ae6a
PKG_SOURCE_URL:=http://www.unix-ag.uni-kl.de/~massar/vpnc/

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,VPNC,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) STAGING_DIR=$(STAGING_DIR) CC=$(TARGET_CC) DESTDIR=$(PKG_INSTALL_DIR) -C $(PKG_BUILD_DIR) all install
	touch $@

$(IPKG_VPNC):
	mkdir -p $(IDIR_VPNC)
	cp -fpR $(PKG_INSTALL_DIR)/* $(IDIR_VPNC)/
	$(RSTRIP) $(IDIR_VPNC)
	$(IPKG_BUILD) $(IDIR_VPNC) $(PACKAGE_DIR)
