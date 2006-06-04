# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=rp-l2tp
PKG_VERSION:=0.4
PKG_RELEASE:=1
PKG_MD5SUM:=0e45d11cb4fa6c56cce6b1d119733ed9

PKG_SOURCE_URL:=@SF/rp-l2tp
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,RP_L2TPD,rp-l2tpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,RP_L2TPD_MOD_CMD,rp-l2tpd-mod-cmd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,RP_L2TPD_MOD_PPP,rp-l2tpd-mod-ppp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
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
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_RP_L2TPD):
	install -d -m0755 $(IDIR_RP_L2TPD)/etc
	$(CP) $(PKG_INSTALL_DIR)/etc/l2tp.conf.example $(IDIR_RP_L2TPD)/etc/l2tp.conf
	install -d -m0755 $(IDIR_RP_L2TPD)/etc/init.d
	install -m0755 ./files/rp-l2tpd.init $(IDIR_RP_L2TPD)/etc/init.d/rp-l2tpd
	install -d -m0755 $(IDIR_RP_L2TPD)/usr/lib/l2tp
	install -d -m0755 $(IDIR_RP_L2TPD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/l2tpd $(IDIR_RP_L2TPD)/usr/sbin/
	$(RSTRIP) $(IDIR_RP_L2TPD)
	$(IPKG_BUILD) $(IDIR_RP_L2TPD) $(PACKAGE_DIR)

$(IPKG_RP_L2TPD_MOD_CMD):
	install -d -m0755 $(IDIR_RP_L2TPD_MOD_CMD)/usr/lib/l2tp
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/l2tp/cmd.so $(IDIR_RP_L2TPD_MOD_CMD)/usr/lib/l2tp/
	install -d -m0755 $(IDIR_RP_L2TPD_MOD_CMD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/l2tp-control $(IDIR_RP_L2TPD_MOD_CMD)/usr/sbin/
	$(RSTRIP) $(IDIR_RP_L2TPD_MOD_CMD)
	$(IPKG_BUILD) $(IDIR_RP_L2TPD_MOD_CMD) $(PACKAGE_DIR)

$(IPKG_RP_L2TPD_MOD_PPP):
	install -d -m0755 $(IDIR_RP_L2TPD_MOD_PPP)/usr/lib/l2tp
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/l2tp/sync-pppd.so $(IDIR_RP_L2TPD_MOD_PPP)/usr/lib/l2tp/
	$(RSTRIP) $(IDIR_RP_L2TPD_MOD_PPP)
	$(IPKG_BUILD) $(IDIR_RP_L2TPD_MOD_PPP) $(PACKAGE_DIR)
