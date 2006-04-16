# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=iptraf
PKG_VERSION:=3.0.0
PKG_RELEASE:=1
PKG_MD5SUM:=377371c28ee3c21a76f7024920649ea8

PKG_SOURCE_URL:=ftp://iptraf.seul.org/pub/iptraf/ \
	ftp://the.wiretapped.net/pub/security/network-monitoring/iptraf/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,IPTRAF,iptraf,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR)/src \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		DEBUG="" \
		INCLUDEDIR="-I../support -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDOPTS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		PLATFORM="-DPLATFORM=\\\"Linux/$(ARCH)\\\"" \
		TARGET="/usr/bin" \
		WORKDIR="/var/lib/iptraf" \
		all
	touch $@

$(IPKG_IPTRAF):
	install -d -m0755 $(IDIR_IPTRAF)/usr/bin \
			  $(IDIR_IPTRAF)/var/run/iptraf \
			  $(IDIR_IPTRAF)/var/lib/iptraf
	install -m0755 $(PKG_BUILD_DIR)/src/iptraf $(IDIR_IPTRAF)/usr/bin/
	$(RSTRIP) $(IDIR_IPTRAF)
	$(IPKG_BUILD) $(IDIR_IPTRAF) $(PACKAGE_DIR)

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
