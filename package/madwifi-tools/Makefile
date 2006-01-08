# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=madwifi
PKG_VERSION:=r1358-20051215
PKG_RELEASE:=1
PKG_MD5SUM:=0ef00a095359e5af313f52446ebfbbf1

PKG_SOURCE_URL:=http://snapshots.madwifi.org/madwifi-ng
PKG_SOURCE:=$(PKG_NAME)-ng-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-ng-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MADWIFI_TOOLS_BASE,madwifi-tools-base,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH),kmod-madwifi))
$(eval $(call PKG_template,MADWIFI_TOOLS_STATS,madwifi-tools-stats,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH),kmod-madwifi))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)/tools \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		all
	touch $@

$(IPKG_MADWIFI_TOOLS_BASE):
	install -d -m0755 $(IDIR_MADWIFI_TOOLS_BASE)/usr/sbin
	install -m0755 \
		$(PKG_BUILD_DIR)/tools/athctrl \
		$(PKG_BUILD_DIR)/tools/athkey \
		$(PKG_BUILD_DIR)/tools/wlanconfig \
		$(IDIR_MADWIFI_TOOLS_BASE)/usr/sbin
	$(RSTRIP) $(IDIR_MADWIFI_TOOLS_BASE)
	$(IPKG_BUILD) $(IDIR_MADWIFI_TOOLS_BASE) $(PACKAGE_DIR)

$(IPKG_MADWIFI_TOOLS_STATS):
	install -d -m0755 $(IDIR_MADWIFI_TOOLS_STATS)/usr/sbin
	install -m0755 \
		$(PKG_BUILD_DIR)/tools/athstats \
		$(PKG_BUILD_DIR)/tools/athdebug \
		$(PKG_BUILD_DIR)/tools/80211stats \
		$(PKG_BUILD_DIR)/tools/80211debug \
		$(IDIR_MADWIFI_TOOLS_STATS)/usr/sbin
	$(RSTRIP) $(IDIR_MADWIFI_TOOLS_STATS)
	$(IPKG_BUILD) $(IDIR_MADWIFI_TOOLS_STATS) $(PACKAGE_DIR)
