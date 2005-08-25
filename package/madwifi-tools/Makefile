# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=madwifi
PKG_VERSION:=2005-08-13
PKG_RELEASE:=1
PKG_MD5SUM:=188127127aebd8815595c025a787d729

PKG_SOURCE_URL:=http://madwifi.otaku42.de/2005/08/
PKG_SOURCE:=$(PKG_NAME)-cvs-snapshot-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

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
		$(IDIR_MADWIFI_TOOLS)/usr/sbin
	$(RSTRIP) $(IDIR_MADWIFI_TOOLS)
	$(IPKG_BUILD) $(IDIR_MADWIFI_TOOLS) $(PACKAGE_DIR)
