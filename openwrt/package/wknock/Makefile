# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=wknock
PKG_VERSION:=1
PKG_RELEASE:=1
PKG_MD5SUM:=6b7a1f3cbb0af88da02e5d30cefd8e7f

PKG_SOURCE_URL:=http://blackhat.com/presentations/bh-europe-05/BH_EU_05-Oudot/
PKG_SOURCE:=$(PKG_NAME).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,WKNOCK,wknock,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib -lpcap" \
		all
	touch $@

$(IPKG_WKNOCK):
	install -d -m0755 $(IDIR_WKNOCK)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/wknock $(IDIR_WKNOCK)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/wknock-ng $(IDIR_WKNOCK)/usr/sbin/
	$(RSTRIP) $(IDIR_WKNOCK)/
	$(IPKG_BUILD) $(IDIR_WKNOCK) $(PACKAGE_DIR)
