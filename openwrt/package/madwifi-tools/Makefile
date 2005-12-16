# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=madwifi
PKG_VERSION:=r1158-20051215
PKG_RELEASE:=1
PKG_MD5SUM:=0ef00a095359e5af313f52446ebfbbf1

PKG_SOURCE_URL:=http://snapshots.madwifi.org/madwifi-ng
PKG_SOURCE:=$(PKG_NAME)-trunk-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-ng-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MADWIFI_TOOLS,madwifi-tools,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH),kmod-madwifi))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)/tools \
		$(TARGET_CONFIGURE_OPTS)
	touch $@

$(IPKG_MADWIFI_TOOLS):
	mkdir -p $(IDIR_MADWIFI_TOOLS)/usr/sbin
	cp -fpR $(PKG_BUILD_DIR)/tools/athstats \
		$(PKG_BUILD_DIR)/tools/athdebug \
		$(PKG_BUILD_DIR)/tools/80211stats \
		$(PKG_BUILD_DIR)/tools/80211debug \
		$(PKG_BUILD_DIR)/tools/athctrl \
		$(PKG_BUILD_DIR)/tools/athkey \
		$(PKG_BUILD_DIR)/tools/wlanconfig \
		$(IDIR_MADWIFI_TOOLS)/usr/sbin
	$(RSTRIP) $(IDIR_MADWIFI_TOOLS)
	$(IPKG_BUILD) $(IDIR_MADWIFI_TOOLS) $(PACKAGE_DIR)
