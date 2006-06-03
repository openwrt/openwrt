# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=aircrack-ng
PKG_VERSION:=0.5
PKG_RELEASE:=1
PKG_MD5SUM:=303daa6f1b030f8f9a2f00542051b96c

PKG_SOURCE_URL:=http://download.aircrack-ng.org
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,AIRCRACK_NG,aircrack-ng,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		prefix="/usr" \
		destdir="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_AIRCRACK_NG):
	install -d -m0755 $(IDIR_AIRCRACK_NG)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/aircrack-ng $(IDIR_AIRCRACK_NG)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/airdecap-ng $(IDIR_AIRCRACK_NG)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/arpforge-ng $(IDIR_AIRCRACK_NG)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ivstools $(IDIR_AIRCRACK_NG)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/kstats $(IDIR_AIRCRACK_NG)/usr/bin/
	install -d -m0755 $(IDIR_AIRCRACK_NG)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/aireplay-ng $(IDIR_AIRCRACK_NG)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/airodump-ng $(IDIR_AIRCRACK_NG)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/airmon-ng $(IDIR_AIRCRACK_NG)/usr/sbin/
	$(RSTRIP) $(IDIR_AIRCRACK_NG)
	$(IPKG_BUILD) $(IDIR_AIRCRACK_NG) $(PACKAGE_DIR)

