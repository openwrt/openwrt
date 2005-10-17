include $(TOPDIR)/rules.mk

PKG_NAME:=aircrack
PKG_VERSION:=2.3
PKG_RELEASE:=1
PKG_MD5SUM:=8c0c88abe107ca26d019d8f43958e60a

PKG_SOURCE_URL:=http://www.cr0.net:8040/code/network/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,AIRCRACK,aircrack,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		CROSS="$(TARGET_CROSS)" \
		prefix="/usr" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_AIRCRACK):
	install -d -m0755 $(IDIR_AIRCRACK)/usr/bin
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/aircrack $(IDIR_AIRCRACK)/usr/bin/
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/airdecap $(IDIR_AIRCRACK)/usr/bin/
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/aireplay $(IDIR_AIRCRACK)/usr/bin/
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/airodump $(IDIR_AIRCRACK)/usr/bin/
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/arpforge $(IDIR_AIRCRACK)/usr/bin/
	$(RSTRIP) $(IDIR_AIRCRACK)
	$(IPKG_BUILD) $(IDIR_AIRCRACK) $(PACKAGE_DIR)

