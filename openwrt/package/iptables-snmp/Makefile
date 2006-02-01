# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=iptables-snmp
PKG_VERSION:=0.1
PKG_RELEASE:=1
PKG_MD5SUM:=8370d2f0c899461a053da491400119d1

PKG_SOURCE_URL:=http://www.nobiscuit.com/iptables-snmp
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,IPTABLES_SNMP,iptables-snmp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -f config.{cache,status}; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) " \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--program-prefix="" \
			--program-suffix="" \
			--prefix=/usr \
			--exec-prefix=/usr \
			--bindir=/usr/bin \
			--datadir=/usr/share \
			--includedir=/usr/include \
			--infodir=/usr/share/info \
			--libdir=/usr/lib \
			--libexecdir=/usr/lib \
			--localstatedir=/var \
			--mandir=/usr/share/man \
			--sbindir=/usr/sbin \
			--sysconfdir=/etc \
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		INSTALL_PREFIX="$(PKG_INSTALL_DIR)" \
		all install
	touch $@
 
$(IPKG_IPTABLES_SNMP):
	install -d -m0755 $(IDIR_IPTABLES_SNMP)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/iptables-snmp.so $(IDIR_IPTABLES_SNMP)/usr/lib
	$(RSTRIP) $(IDIR_IPTABLES_SNMP)
	$(IPKG_BUILD) $(IDIR_IPTABLES_SNMP) $(PACKAGE_DIR)
 
mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built