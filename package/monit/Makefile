# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=monit
PKG_VERSION:=4.6
PKG_RELEASE:=1
PKG_MD5SUM:=4e53aa44e4ca264e61c7c401cee4e697

PKG_SOURCE_URL:=http://www.tildeslash.com/monit/dist/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MONIT,monit,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,MONIT_NOSSL,monit-nossl,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
ifneq ($(BR2_PACKAGE_MONIT),)
	( cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--program-prefix="" \
			--program-suffix="" \
			--prefix=/usr \
			--exec-prefix=/usr \
			--bindir=/usr/sbin \
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
			--with-ssl \
			--with-ssl-dir="$(STAGING_DIR)/usr" \
	);
	$(MAKE) -C $(PKG_BUILD_DIR)
	( cd $(PKG_BUILD_DIR); mv monit monit-ssl; )
endif
ifneq ($(BR2_PACKAGE_MONIT),)
	( cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--program-prefix="" \
			--program-suffix="" \
			--prefix=/usr \
			--exec-prefix=/usr \
			--bindir=/usr/sbin \
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
			--without-ssl \
	);
	$(MAKE) -C $(PKG_BUILD_DIR)
	( cd $(PKG_BUILD_DIR); mv monit monit-nossl; )
	$(MAKE) -C $(PKG_BUILD_DIR) clean
endif
	touch $@

$(IPKG_MONIT):
	install -d -m0755 $(IDIR_MONIT)/etc
	install -m0600 $(PKG_BUILD_DIR)/monitrc $(IDIR_MONIT)/etc/
	install -d -m0755 $(IDIR_MONIT)/etc/init.d
	install -m0755 ./files/monit.init $(IDIR_MONIT)/etc/init.d/monit
	install -d -m0755 $(IDIR_MONIT)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/monit-ssl $(IDIR_MONIT)/usr/sbin/monit
	$(RSTRIP) $(IDIR_MONIT)
	$(IPKG_BUILD) $(IDIR_MONIT) $(PACKAGE_DIR)

$(IPKG_MONIT_NOSSL):
	install -d -m0755 $(IDIR_MONIT_NOSSL)/etc
	install -m0600 $(PKG_BUILD_DIR)/monitrc $(IDIR_MONIT_NOSSL)/etc/
	install -d -m0755 $(IDIR_MONIT_NOSSL)/etc/init.d
	install -m0755 ./files/monit.init $(IDIR_MONIT_NOSSL)/etc/init.d/monit
	install -d -m0755 $(IDIR_MONIT_NOSSL)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/monit-nossl $(IDIR_MONIT_NOSSL)/usr/sbin/monit
	$(RSTRIP) $(IDIR_MONIT_NOSSL)
	$(IPKG_BUILD) $(IDIR_MONIT_NOSSL) $(PACKAGE_DIR)
