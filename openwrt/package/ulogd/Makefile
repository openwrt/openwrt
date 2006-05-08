# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ulogd
PKG_VERSION:=1.24
PKG_RELEASE:=1
PKG_MD5SUM:=05b4ed2926b9a22aaeaf642917bbf8ff

PKG_SOURCE_URL:=ftp://ftp.netfilter.org/pub/ulogd/ \
	ftp://ftp.be.netfilter.org/pub/netfilter/ulogd/ \
	ftp://ftp.de.netfilter.org/pub/netfilter/ulogd/ \
	ftp://ftp.no.netfilter.org/pub/netfilter/ulogd/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

ifneq ($(BR2_PACKAGE_ULOGD_MOD_MYSQL),)
ULOGD_MYSQL_OPTION:=--with-mysql=$(STAGING_DIR)/usr
endif

ifneq ($(BR2_PACKAGE_ULOGD_MOD_PCAP),)
ULOGD_PCAP_HEADER:=yes
else
ULOGD_PCAP_HEADER:=no
endif

ifneq ($(BR2_PACKAGE_ULOGD_MOD_PGSQL),)
ULOGD_PGSQL_OPTION:=--with-pgsql=$(STAGING_DIR)/usr
endif

ifneq ($(BR2_PACKAGE_ULOGD_MOD_SQLITE),)
ULOGD_SQLITE_OPTION:=--with-sqlite3=$(STAGING_DIR)/usr
endif

define IPKG_plugin_template

$$(IPKG_$(1)):
	install -m0755 -d $$(IDIR_$(1))/usr/lib/ulogd
	for m in $(2); do \
		$(CP) $(PKG_INSTALL_DIR)/usr/lib/ulogd/ulogd_$$$${m}.so $$(IDIR_$(1))/usr/lib/ulogd/ ; \
	done
	$(RSTRIP) $$(IDIR_$(1))
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)

endef

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,ULOGD,ulogd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ULOGD_MOD_MYSQL,ulogd-mod-mysql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ULOGD_MOD_PCAP,ulogd-mod-pcap,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ULOGD_MOD_PGSQL,ulogd-mod-pgsql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ULOGD_MOD_SQLITE,ulogd-mod-sqlite,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ULOGD_MOD_EXTRA,ulogd-mod-extra,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(eval $(call IPKG_plugin_template,ULOGD_MOD_MYSQL,MYSQL))
$(eval $(call IPKG_plugin_template,ULOGD_MOD_PCAP,PCAP))
$(eval $(call IPKG_plugin_template,ULOGD_MOD_PGSQL,PGSQL))
$(eval $(call IPKG_plugin_template,ULOGD_MOD_SQLITE,SQLITE3))
$(eval $(call IPKG_plugin_template,ULOGD_MOD_EXTRA,LOCAL LOGEMU OPRINT PWSNIFF SYSLOG))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		ac_cv_header_pcap_h=$(ULOGD_PCAP_HEADER) \
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
			--enable-shared \
			--disable-static \
			$(ULOGD_MYSQL_OPTION) \
			$(ULOGD_PGSQL_OPTION) \
			$(ULOGD_SQLITE_OPTION) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		STAGING_DIR=$(STAGING_DIR) \
		all install
	touch $@

$(IPKG_ULOGD):
	install -d -m0755 $(IDIR_ULOGD)/etc
	$(CP) $(PKG_INSTALL_DIR)/etc/ulogd.conf $(IDIR_ULOGD)/etc/
	install -d -m0755 $(IDIR_ULOGD)/etc/default
	install -m0644 ./files/ulogd.default $(IDIR_ULOGD)/etc/default/ulogd
	install -d -m0755 $(IDIR_ULOGD)/etc/init.d
	install -m0755 ./files/ulogd.init $(IDIR_ULOGD)/etc/init.d/ulogd
	ln -sf ulogd $(IDIR_ULOGD)/etc/init.d/S49ulogd
	install -d -m0755 $(IDIR_ULOGD)/usr/lib/ulogd
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/ulogd/ulogd_BASE.so $(IDIR_ULOGD)/usr/lib/ulogd/
	install -d -m0755 $(IDIR_ULOGD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/ulogd $(IDIR_ULOGD)/usr/sbin/
	$(RSTRIP) $(IDIR_ULOGD)
	$(IPKG_BUILD) $(IDIR_ULOGD) $(PACKAGE_DIR)

