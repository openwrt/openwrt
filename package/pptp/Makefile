# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=pptp
PKG_VERSION:=1.6.0
PKG_RELEASE:=3
PKG_MD5SUM:=9a706327fb9827541d7c86d48ceb9631

PKG_SOURCE_URL:=@SF/pptpclient
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PPTP,pptp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		CFLAGS="$(TARGET_CFLAGS)" \
		all
	touch $@

$(IPKG_PPTP):
	install -d -m0755 $(IDIR_PPTP)/sbin
	install -m0755 ./files/ifup.pptp $(IDIR_PPTP)/sbin/ifup.pptp
	install -d -m0755 $(IDIR_PPTP)/etc/ppp
	install -m0644 ./files/options.pptp $(IDIR_PPTP)/etc/ppp/
	install -d -m0755 $(IDIR_PPTP)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/pptp $(IDIR_PPTP)/usr/sbin/
	$(RSTRIP) $(IDIR_PPTP)
	$(IPKG_BUILD) $(IDIR_PPTP) $(PACKAGE_DIR)
