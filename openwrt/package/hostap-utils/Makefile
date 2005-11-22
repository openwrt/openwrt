# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=hostap-utils
PKG_VERSION:=0.4.7
PKG_RELEASE:=1
PKG_MD5SUM:=afe041581b8f01666e353bec20917c85

PKG_SOURCE_URL:=http://hostap.epitest.fi/releases/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,HOSTAP_UTILS,hostap-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include -Wall" \
		all
	touch $@

$(IPKG_HOSTAP_UTILS):
	install -m0755 -d $(IDIR_HOSTAP_UTILS)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/hostap_crypt_conf $(IDIR_HOSTAP_UTILS)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/hostap_diag $(IDIR_HOSTAP_UTILS)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/hostap_io_debug $(IDIR_HOSTAP_UTILS)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/hostap_rid $(IDIR_HOSTAP_UTILS)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/prism2_srec $(IDIR_HOSTAP_UTILS)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/split_combined_hex $(IDIR_HOSTAP_UTILS)/usr/sbin/
	$(RSTRIP) $(IDIR_HOSTAP_UTILS)
	$(IPKG_BUILD) $(IDIR_HOSTAP_UTILS) $(PACKAGE_DIR)

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built

