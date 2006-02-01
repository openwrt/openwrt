# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=l2tpns
PKG_VERSION:=2.1.14
PKG_RELEASE:=1
PKG_MD5SUM:=2a0ee2a3678160c335f1d68c17c4f871

PKG_SOURCE_URL:=@SF/l2tpns
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,L2TPNS,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		LD=$(TARGET_CC) \
		OPTIM="$(TARGET_CFLAGS)" \
		DESTDIR=$(PKG_INSTALL_DIR) \
		STAGING_DIR=$(STAGING_DIR) \
		all install
	touch $@

$(IPKG_L2TPNS):
	install -d -m0755 $(IDIR_L2TPNS)/etc/l2tpns
	$(CP) $(PKG_INSTALL_DIR)/etc/l2tpns/* $(IDIR_L2TPNS)/etc/l2tpns/
	install -d -m0755 $(IDIR_L2TPNS)/usr/lib/l2tpns
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/l2tpns/* $(IDIR_L2TPNS)/usr/lib/l2tpns/
	install -d -m0755 $(IDIR_L2TPNS)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/* $(IDIR_L2TPNS)/usr/sbin/
	$(RSTRIP) $(IDIR_L2TPNS)
	$(IPKG_BUILD) $(IDIR_L2TPNS) $(PACKAGE_DIR)
