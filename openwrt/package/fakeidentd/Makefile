# $Id: $

include $(TOPDIR)/rules.mk

PKG_NAME:=fakeidentd
PKG_VERSION:=2.2
PKG_RELEASE:=1
PKG_MD5SUM:=3aaf9d525da08ae18dd4133de4ff7be3

PKG_SOURCE_URL:=@openwrt/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,FAKEIDENTD,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_BUILD_DIR)/identd.c
	touch $@

$(IPKG_FAKEIDENTD):
	install -d -m0755 $(IDIR_FAKEIDENTD)/usr/sbin \
			  $(IDIR_FAKEIDENTD)/etc/init.d \
			  $(IDIR_FAKEIDENTD)/etc/default/
	echo $(whoami) > $(IDIR_FAKEIDENTD)/etc/default/$(PKG_NAME)
	$(CP) $(PKG_BUILD_DIR)/fakeidentd $(IDIR_FAKEIDENTD)/usr/sbin/
	$(CP) ./files/$(PKG_NAME).init $(IDIR_FAKEIDENTD)/etc/init.d/$(PKG_NAME)
	$(RSTRIP) $(IDIR_FAKEIDENTD)
	$(IPKG_BUILD) $(IDIR_FAKEIDENTD) $(PACKAGE_DIR)

