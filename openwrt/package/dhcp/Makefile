# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=dhcp
PKG_VERSION:=3.0.3
PKG_RELEASE:=1
PKG_MD5SUM:=f91416a0b8ed3fd0601688cf0b7df58f

PKG_SOURCE_URL:=ftp://ftp.isc.org/isc/dhcp/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,DHCP_RELAY,dhcp-relay,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,DHCP_SERVER,dhcp-server,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR) ; \
		./configure \
		  --copts "$(TARGET_CFLAGS)" \
		  linux-2.2 \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_DHCP_RELAY):
	install -d -m0755 $(IDIR_DHCP_RELAY)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/dhcrelay $(IDIR_DHCP_RELAY)/usr/sbin/
	$(RSTRIP) $(IDIR_DHCP_RELAY)
	$(IPKG_BUILD) $(IDIR_DHCP_RELAY) $(PACKAGE_DIR)

$(IPKG_DHCP_SERVER):
	install -d -m0755 $(IDIR_DHCP_SERVER)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/dhcpd $(IDIR_DHCP_SERVER)/usr/sbin/
	$(RSTRIP) $(IDIR_DHCP_SERVER)
	$(IPKG_BUILD) $(IDIR_DHCP_SERVER) $(PACKAGE_DIR)
