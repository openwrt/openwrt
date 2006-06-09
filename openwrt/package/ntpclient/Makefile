# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ntpclient
PKG_VERSION:=2003_194
PKG_RELEASE:=2
PKG_MD5SUM:=94e84d5c6023c5e1f3890b28d0a08c92

PKG_SOURCE_URL:=http://doolittle.faludi.com/ntpclient
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,NTPCLIENT,ntpclient,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) CFLAGS="$(TARGET_CFLAGS)" all adjtimex
	touch $@

$(IPKG_NTPCLIENT):
	install -d -m0755 $(IDIR_NTPCLIENT)/etc/hotplug.d/iface
	install -m0755 ./files/ntpclient.init $(IDIR_NTPCLIENT)/etc/hotplug.d/iface/10-ntpclient
	install -d -m0755 $(IDIR_NTPCLIENT)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/ntpclient $(IDIR_NTPCLIENT)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/adjtimex $(IDIR_NTPCLIENT)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/rate.awk $(IDIR_NTPCLIENT)/usr/sbin/
	$(RSTRIP) $(IDIR_NTPCLIENT)
	$(IPKG_BUILD) $(IDIR_NTPCLIENT) $(PACKAGE_DIR)

