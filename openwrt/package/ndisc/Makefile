# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ndisc6
PKG_VERSION:=0.5.1
PKG_RELEASE:=1
PKG_MD5SUM:=33487fe5505b195eb31468d88ba9ab56

PKG_SOURCE_URL:=http://people.via.ecp.fr/~rem/ndisc6/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,NDISC6,ndisc6,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,RDISC6,rdisc6,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,TCPTRACEROUTE6,tcptraceroute6,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		all
	touch $@

$(IPKG_NDISC6):
	install -d -m0755 $(IDIR_NDISC6)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/ndisc6 $(IDIR_NDISC6)/usr/bin/
	$(RSTRIP) $(IDIR_NDISC6)
	$(IPKG_BUILD) $(IDIR_NDISC6) $(PACKAGE_DIR)

$(IPKG_RDISC6):
	install -d -m0755 $(IDIR_RDISC6)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/rdisc6 $(IDIR_RDISC6)/usr/bin/
	$(RSTRIP) $(IDIR_RDISC6)
	$(IPKG_BUILD) $(IDIR_RDISC6) $(PACKAGE_DIR)

$(IPKG_TCPTRACEROUTE6):
	install -d -m0755 $(IDIR_TCPTRACEROUTE6)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/tcptraceroute6 $(IDIR_TCPTRACEROUTE6)/usr/bin/
	$(RSTRIP) $(IDIR_TCPTRACEROUTE6)
	$(IPKG_BUILD) $(IDIR_TCPTRACEROUTE6) $(PACKAGE_DIR)
