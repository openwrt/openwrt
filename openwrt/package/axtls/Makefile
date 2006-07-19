# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=axtls
PKG_VERSION:=0.95
PKG_RELEASE:=1
PKG_MD5SUM:=cd887f379f303dd3ef691d8641307f0e

PKG_SOURCE:=axTLS-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.leroc.com.au/axTLS/
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/axTLS

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBAXTLS,libaxtls,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,AWHTTPD,awhttpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	$(CP) ./files/config $(PKG_BUILD_DIR)/config/.config
	$(MAKE) -C $(PKG_BUILD_DIR) oldconfig
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		STRIP=/bin/true \
		OPT_CFLAGS="$(TARGET_CFLAGS)" OPT_LDFLAGS=""
	touch $@

$(IPKG_LIBAXTLS):
	install -d -m0755 $(IDIR_LIBAXTLS)/usr/lib
	install -m0755 $(PKG_BUILD_DIR)/libaxtls.so $(IDIR_LIBAXTLS)/usr/lib/
	$(RSTRIP) $(IDIR_LIBAXTLS)
	$(IPKG_BUILD) $(IDIR_LIBAXTLS) $(PACKAGE_DIR)

$(IPKG_AWHTTPD):
	install -d -m0755 $(IDIR_AWHTTPD)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/awhttpd $(IDIR_AWHTTPD)/usr/sbin/
	$(RSTRIP) $(IDIR_AWHTTPD)
	$(IPKG_BUILD) $(IDIR_AWHTTPD) $(PACKAGE_DIR)
