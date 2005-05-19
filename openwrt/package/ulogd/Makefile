# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ulogd
PKG_VERSION:=1.23
PKG_RELEASE:=1
PKG_MD5SUM:=fa3dfcaacf31855626d5b731b04a077f

PKG_SOURCE_URL:=ftp://ftp.netfilter.org/pub/ulogd/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

ifneq ($(BR2_PACKAGE_ULOGD_MYSQL_PLUGIN),)
ULOGD_MYSQL_OPTION:=--with-mysql=$(STAGING_DIR)/usr
endif

ifneq ($(BR2_PACKAGE_ULOGD_PGSQL_PLUGIN),)
ULOGD_PGSQL_OPTION:=--with-pgsql=$(STAGING_DIR)/usr
endif

define IPKG_plugin_template

$$(IPKG_$(1)):
	install -m0755 -d $$(IDIR_$(1))/usr/lib/ulogd
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/ulogd/ulogd_$(2).so $$(IDIR_$(1))/usr/lib/ulogd/
	$(RSTRIP) $$(IDIR_$(1))
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)

endef

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,ULOGD,ulogd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ULOGD_MYSQL_PLUGIN,ulogd-mod-mysql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ULOGD_PGSQL_PLUGIN,ulogd-mod-pgsql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(eval $(call IPKG_plugin_template,ULOGD_MYSQL_PLUGIN,MYSQL))
$(eval $(call IPKG_plugin_template,ULOGD_PGSQL_PLUGIN,PGSQL))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		ac_cv_header_pcap_h=no \
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
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	touch $(PKG_BUILD_DIR)/.built

$(IPKG_ULOGD):
	install -d -m0755 $(IDIR_ULOGD)/etc
	cp -fpR $(PKG_INSTALL_DIR)/etc/ulogd.conf $(IDIR_ULOGD)/etc/
	install -d -m0755 $(IDIR_ULOGD)/etc/default
	install -m0644 ./files/ulogd.default $(IDIR_ULOGD)/etc/default/ulogd
	install -d -m0755 $(IDIR_ULOGD)/etc/init.d
	install -m0755 ./files/ulogd.init $(IDIR_ULOGD)/etc/init.d/ulogd
	install -d -m0755 $(IDIR_ULOGD)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/ulogd $(IDIR_ULOGD)/usr/sbin/
	$(RSTRIP) $(IDIR_ULOGD)
	$(IPKG_BUILD) $(IDIR_ULOGD) $(PACKAGE_DIR)
