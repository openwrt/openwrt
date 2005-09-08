# $Id$
#
include $(TOPDIR)/rules.mk

PKG_NAME:=ipset
PKG_VERSION:=2.2.2
PKG_RELEASE:=1
PKG_MD5SUM:=e760bc3ba45420118086428af4fd3b61

PKG_SOURCE_URL:=http://ipset.netfilter.org
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-20050720.tar.bz2
PKG_BUILD_DIR:=$(BUILD_DIR)/ipset-$(PKG_VERSION)
PKG_CAT:=bzcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,IPSET,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld \
		CFLAGS="$(TARGET_CFLAGS) -Wall -I$(LINUX_DIR)/include -I." \
		KERNEL_DIR=$(LINUX_DIR) PREFIX=/usr
	touch $@

$(IPKG_IPSET):
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR=$(IDIR_IPSET) PREFIX=/usr install
	rm -rf $(IDIR_IPSET)/usr/man
	$(IPKG_BUILD) $(IDIR_IPSET) $(PACKAGE_DIR)

