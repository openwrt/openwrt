# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=scanlogd
PKG_VERSION:=2.2.6
PKG_RELEASE:=1
PKG_MD5SUM:=7b8187ea718ebe47f22805b921b909ab

PKG_SOURCE_URL:=http://www.openwall.com/scanlogd/ \
		ftp://ftp.wiretapped.net/pub/openwall/ \
		http://distro.ibiblio.org/pub/linux/distributions/openwall/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,SCANLOGD,scanlogd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	$(SED) "s,/var/empty,/tmp/.scanlogd," $(PKG_BUILD_DIR)/params.h
	$(SED) "s,#undef SCANLOGD_DEVICE,#define SCANLOGD_DEVICE \"all\"," $(PKG_BUILD_DIR)/params.h
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CC=$(TARGET_CC) LD=$(TARGET_CC) CFLAGS="-c $(TARGET_CFLAGS)" \
		LDFLAGS= \
		PCAP_H="-I$(STAGING_DIR)/usr/include" \
		NIDS_H="-I$(STAGING_DIR)/usr/include" \
		NIDS_L="-L$(STAGING_DIR)/usr/lib -lnids -lnet -lpcap" \
		libnids
	touch $@

$(IPKG_SCANLOGD):
	install -d -m0755 $(IDIR_SCANLOGD)/etc/init.d
	install -m0755 ./files/scanlogd.init $(IDIR_SCANLOGD)/etc/init.d/S60scanlogd
	install -d -m0755 $(IDIR_SCANLOGD)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_SCANLOGD)/usr/sbin/
	$(RSTRIP) $(IDIR_SCANLOGD)
	$(IPKG_BUILD) $(IDIR_SCANLOGD) $(PACKAGE_DIR)
