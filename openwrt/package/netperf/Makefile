# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=netperf
PKG_VERSION:=2.3pl1
PKG_RELEASE:=1
PKG_MD5SUM:=b74314d78af31cb13516fb9a372d2e86

PKG_SOURCE_URL:=ftp://ftp.netperf.org/netperf/archive/ \
	ftp://ftp.cup.hp.com/dist/networking/benchmarks/netperf/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,NETPERF,netperf,$(PKG_VERSION)$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
	    TARGET_NETPERF_HOME="/etc/netperf.conf" \
	    TARGET_CC=$(TARGET_CROSS)gcc \
	    TARGET_CFLAGS="$(TARGET_CFLAGS)" \
		TARGET_LIBS="" \
		all
	touch $@

$(IPKG_NETPERF):
	install -d -m0755 $(IDIR_NETPERF)/etc/init.d
	install -m0755 ./files/netserver.init $(IDIR_NETPERF)/etc/init.d/S51netserver
	install -d -m0755 $(IDIR_NETPERF)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/netperf $(IDIR_NETPERF)/usr/bin/
	install -m0755 $(PKG_BUILD_DIR)/netserver $(IDIR_NETPERF)/usr/bin/
	$(RSTRIP) $(IDIR_NETPERF)
	$(IPKG_BUILD) $(IDIR_NETPERF) $(PACKAGE_DIR)
