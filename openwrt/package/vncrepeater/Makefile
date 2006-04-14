# $Id: Makefile 3112 2006-02-01 23:53:19Z mbm $

include $(TOPDIR)/rules.mk

PKG_NAME:=vncrepeater
PKG_VERSION:=0.12
PKG_RELEASE:=1
PKG_MD5SUM:=5951740a8a103d7d9668558b3639c6c4

PKG_SOURCE_URL:=http://www.rit.edu/~reh5586/openwrt/packages/vncrepeater/src
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,VNCREPEATER,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		CFLAGS="$(TARGET_CFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		repeater
	touch $@

$(IPKG_VNCREPEATER):
	install -d -m0755 $(IDIR_VNCREPEATER)/usr/sbin $(IDIR_VNCREPEATER)/etc
	$(CP) $(PKG_BUILD_DIR)/repeater $(IDIR_VNCREPEATER)/usr/sbin/$(PKG_NAME)
	install -d -m0755 $(IDIR_VNCREPEATER)/etc
	$(CP) $(PKG_BUILD_DIR)/uvncrepeater.ini $(IDIR_VNCREPEATER)/etc/vncrepeater.conf
	install -d -m0755 $(IDIR_VNCREPEATER)/etc/init.d
	$(CP) $(PKG_BUILD_DIR)/S60vncrepeater $(IDIR_VNCREPEATER)/etc/init.d/
	$(RSTRIP) $(IDIR_VNCREPEATER)
	$(IPKG_BUILD) $(IDIR_VNCREPEATER) $(PACKAGE_DIR)
