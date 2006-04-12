# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=logrotate
PKG_VERSION:=3.7.1
PKG_RELEASE:=1
PKG_MD5SUM:=552639142e163745f6bcd4f1f3816d8a

PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/l/logrotate
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LOGROTATE,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	touch $@

$(PKG_BUILD_DIR)/.built:
	make -C ${PKG_BUILD_DIR} \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		CC=$(TARGET_CC) logrotate
	touch $@

$(IPKG_LOGROTATE):
	mkdir -p $(IDIR_LOGROTATE)/usr/sbin
	$(CP) ${PKG_BUILD_DIR}/logrotate $(IDIR_LOGROTATE)/usr/sbin
	mkdir -p $(IDIR_LOGROTATE)/etc/logrotate.d
	$(CP) ./files/logrotate.conf $(IDIR_LOGROTATE)/etc
	$(RSTRIP) $(IDIR_LOGROTATE)
	$(IPKG_BUILD) $(IDIR_LOGROTATE) $(PACKAGE_DIR)

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built
