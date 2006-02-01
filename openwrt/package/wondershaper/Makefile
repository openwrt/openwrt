# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=wondershaper
PKG_VERSION:=1.1a
PKG_RELEASE:=1
PKG_MD5SUM:=bbc5a3a4485ab286e337ce8550e7b990

PKG_SOURCE_URL:=http://lartc.org/wondershaper/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,WONDERSHAPER,wondershaper,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	mkdir -p $(PKG_INSTALL_DIR)
	touch $@

$(IPKG_WONDERSHAPER):
	install -d -m0755 $(IDIR_WONDERSHAPER)/usr/sbin
	install -d -m0755 $(IDIR_WONDERSHAPER)/etc/init.d
	$(CP) $(PKG_BUILD_DIR)/wshaper.htb $(IDIR_WONDERSHAPER)/usr/sbin/wshaper
	$(CP) $(PKG_BUILD_DIR)/wshaper.conf $(IDIR_WONDERSHAPER)/etc/wshaper.conf
	ln -fs /usr/sbin/wshaper $(IDIR_WONDERSHAPER)/etc/init.d/S60wondershaper
	$(IPKG_BUILD) $(IDIR_WONDERSHAPER) $(PACKAGE_DIR)

