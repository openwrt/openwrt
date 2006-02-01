# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=dhcp6
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_MD5SUM:=86193dfa62137db3ea459543db4f1102
PKG_SOURCE_URL:=@SF/dhcpv6-linux
PKG_SOURCE:=dhcpv6-linux-$(PKG_VERSION).zip
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=unzip

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,DHCP6,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) CC=$(TARGET_CC) STAGING_DIR=$(STAGING_DIR) -C $(PKG_BUILD_DIR)/server
	$(MAKE) CC=$(TARGET_CC) STAGING_DIR=$(STAGING_DIR) -C $(PKG_BUILD_DIR)/client
	install -d $(PKG_INSTALL_DIR)/usr/sbin $(PKG_INSTALL_DIR)/etc/dhcp{d6,6}
	install -m0755 $(PKG_BUILD_DIR)/client/cli $(PKG_INSTALL_DIR)/usr/sbin/dhcp6client
	install -m0755 $(PKG_BUILD_DIR)/server/serv $(PKG_INSTALL_DIR)/usr/sbin/dhcpd6
	install -m0755 $(PKG_BUILD_DIR)/server/*.conf $(PKG_INSTALL_DIR)/etc/dhcpd6/
	install -m0755 $(PKG_BUILD_DIR)/client/*.conf $(PKG_INSTALL_DIR)/etc/dhcp6/
	touch $@

$(IPKG_DHCP6):
	mkdir -p $(IDIR_DHCP6)
	$(CP) $(PKG_INSTALL_DIR)/* $(IDIR_DHCP6)/
	$(RSTRIP) $(IDIR_DHCP6)
	$(IPKG_BUILD) $(IDIR_DHCP6) $(PACKAGE_DIR)
