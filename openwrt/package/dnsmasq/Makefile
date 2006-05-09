# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=dnsmasq
PKG_VERSION:=2.27
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://thekelleys.org.uk/dnsmasq
PKG_MD5SUM:=489198ec87101087043adc98bbe062dc
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

define Package/dnsmasq
SECTION:=base
CATEGORY:=Network
DEFAULT:=y
TITLE:=A lightweight DNS and DHCP server
DESCRIPTION:=A lightweight DNS and DHCP server.  It is intended to provide \\\
coupled DNS and DHCP service to a LAN.
URL:=http://www.thekelleys.org.uk/dnsmasq/
endef

define Package/dnsmasq/install
	install -d -m0755 $(IDIR_DNSMASQ)/usr/sbin
	install -d -m0755 $(IDIR_DNSMASQ)/etc/init.d/
	install -m0755 $(PKG_BUILD_DIR)/src/dnsmasq $(IDIR_DNSMASQ)/usr/sbin/
	install -m0644 ./files/dnsmasq.conf $(IDIR_DNSMASQ)/etc/dnsmasq.conf
	install -m0755 ./files/dnsmasq.init $(IDIR_DNSMASQ)/etc/init.d/S50dnsmasq
	$(RSTRIP) $(IDIR_DNSMASQ)
	$(IPKG_BUILD) $(IDIR_DNSMASQ) $(PACKAGE_DIR)
endef

$(eval $(call BuildPackage,dnsmasq))
