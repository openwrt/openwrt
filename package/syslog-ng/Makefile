# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=syslog-ng
PKG_VERSION:=1.6.9
PKG_RELEASE:=1
PKG_MD5SUM:=5280a86b4f5c25b2ba157c8cea302cf0

PKG_SOURCE_URL:=http://www.balabit.com/downloads/syslog-ng/stable/src \
		http://www.balabit.com/downloads/syslog-ng/1.6/src
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install
PKG_INIT_LEVEL:=60

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,SYSLOG_NG,syslog-ng,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/libol" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/usr/lib/libol" \
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
			--disable-dependency-tracking \
			--disable-full-static \
			--enable-tcp-wrapper \
			--with-libol="$(STAGING_DIR)/usr/bin" \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_SYSLOG_NG):
	install -d -m0755 $(IDIR_SYSLOG_NG)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/syslog-ng $(IDIR_SYSLOG_NG)/usr/sbin/
	install -d -m0755 $(IDIR_SYSLOG_NG)/etc/init.d
	install -m0755 ./files/syslog-ng.init $(IDIR_SYSLOG_NG)/etc/init.d/S$(PKG_INIT_LEVEL)syslog-ng
	install -d -m0755 $(IDIR_SYSLOG_NG)/etc/syslog-ng
	install -m0644 ./files/syslog-ng.conf $(IDIR_SYSLOG_NG)/etc/syslog-ng/
	$(RSTRIP) $(IDIR_SYSLOG_NG)
	$(IPKG_BUILD) $(IDIR_SYSLOG_NG) $(PACKAGE_DIR)

