# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=pmacct
PKG_VERSION:=0.10.0
PKG_RELEASE:=1
PKG_MD5SUM:=ed510e7ee2130d1f37f27901b89e985a

PKG_SOURCE_URL:=http://www.ba.cnr.it/~paolo/pmacct/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)rc2.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

PMACCT_BASIC_CONFIGURE_OPTS := \

PMACCT_MYSQL_CONFIGURE_OPTS := \
	--enable-mysql \
	--with-mysql-includes="$(STAGING_DIR)/usr/include" \
	--with-mysql-libs="$(STAGING_DIR)/usr/lib/mysql"

PMACCT_PGSQL_CONFIGURE_OPTS := \
	--enable-pgsql \
	--with-pgsql-includes="$(STAGING_DIR)/usr/include" \
	--with-pgsql-libs="$(STAGING_DIR)/usr/lib"

PMACCT_SQLITE_CONFIGURE_OPTS := \
	--enable-sqlite3 \
	--with-sqlite3-includes="$(STAGING_DIR)/usr/include" \
	--with-sqlite3-libs="$(STAGING_DIR)/usr/lib"

NFACCTD_CUSTOM_CONFIGURE_OPTS := \

ifeq ($(BR2_PACKAGE_NFACCTD_ENABLE_DEBUG),y)
NFACCTD_CUSTOM_CONFIGURE_OPTS += --enable-debug
endif

ifeq ($(BR2_PACKAGE_NFACCTD_ENABLE_IPV6),y)
NFACCTD_CUSTOM_CONFIGURE_OPTS += --enable-ipv6
endif

ifeq ($(BR2_PACKAGE_NFACCTD_WITH_MYSQL),y)
NFACCTD_CUSTOM_CONFIGURE_OPTS += $(PMACCT_MYSQL_CONFIGURE_OPTS)
endif

ifeq ($(BR2_PACKAGE_NFACCTD_WITH_PGSQL),y)
NFACCTD_CUSTOM_CONFIGURE_OPTS += $(PMACCT_PGSQL_CONFIGURE_OPTS)
endif

ifeq ($(BR2_PACKAGE_NFACCTD_WITH_SQLITE),y)
NFACCTD_CUSTOM_CONFIGURE_OPTS += $(PMACCT_SQLITE_CONFIGURE_OPTS)
endif

PMACCTD_CUSTOM_CONFIGURE_OPTS := \

ifeq ($(BR2_PACKAGE_PMACCTD_ENABLE_DEBUG),y)
PMACCTD_CUSTOM_CONFIGURE_OPTS += --enable-debug
endif

ifeq ($(BR2_PACKAGE_PMACCTD_ENABLE_IPV6),y)
PMACCTD_CUSTOM_CONFIGURE_OPTS += --enable-ipv6
endif

ifeq ($(BR2_PACKAGE_PMACCTD_WITH_MYSQL),y)
PMACCTD_CUSTOM_CONFIGURE_OPTS += $(PMACCT_MYSQL_CONFIGURE_OPTS)
endif

ifeq ($(BR2_PACKAGE_PMACCTD_WITH_PGSQL),y)
PMACCTD_CUSTOM_CONFIGURE_OPTS += $(PMACCT_PGSQL_CONFIGURE_OPTS)
endif

ifeq ($(BR2_PACKAGE_PMACCTD_WITH_SQLITE),y)
PMACCTD_CUSTOM_CONFIGURE_OPTS += $(PMACCT_SQLITE_CONFIGURE_OPTS)
endif

define PKG_build

ifneq ($(BR2_PACKAGE_$(1)),)
BUILD_TARGETS += $(PKG_BUILD_DIR)/$(2)
BUILD_TARGETS += $(PKG_BUILD_DIR)/$(3)
endif

$(PKG_BUILD_DIR)/$(2) $(PKG_BUILD_DIR)/$(3): $(PKG_BUILD_DIR)/.prepared
	-$(MAKE) -C $(PKG_BUILD_DIR) distclean
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
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
			--libexecdir=/usr/lib/locate \
			--localstatedir=/var/lib \
			--mandir=/usr/share/man \
			--sbindir=/usr/sbin \
			--sysconfdir=/etc \
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
			--enable-shared \
			--disable-static \
			--with-pcap-includes="$(STAGING_DIR)/usr/include" \
			--with-pcap-libs="$(STAGING_DIR)/usr/lib" \
			$$($(1)_CONFIGURE_OPTS) \
	);
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include"
	mv -f $(PKG_BUILD_DIR)/nfacctd $(PKG_BUILD_DIR)/$(2)
	mv -f $(PKG_BUILD_DIR)/pmacctd $(PKG_BUILD_DIR)/$(3)

endef

define PKG_package

$$(IPKG_$(1)): $(PKG_BUILD_DIR)/$(2)
	install -d -m0755 $$(IDIR_$(1))/etc
	install -m0644 ./files/$(3).conf $$(IDIR_$(1))/etc/
	install -d -m0755 $$(IDIR_$(1))/etc/default
	install -m0644 ./files/$(3).default $$(IDIR_$(1))/etc/default/$(3)
	install -d -m0755 $$(IDIR_$(1))/etc/init.d
	install -m0755 ./files/$(3).init $$(IDIR_$(1))/etc/init.d/$(3)
	install -d -m0755 $$(IDIR_$(1))/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/$(2) $$(IDIR_$(1))/usr/sbin/$(3)
	$(RSTRIP) $$(IDIR_$(1))
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)

endef

$(eval $(call PKG_template,NFACCTD_BASIC,nfacctd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,NFACCTD_MYSQL,nfacctd-mysql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,NFACCTD_PGSQL,nfacctd-pgsql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,NFACCTD_SQLITE,nfacctd-sqlite,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,NFACCTD_CUSTOM,nfacctd-custom,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PMACCTD_BASIC,pmacctd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PMACCTD_MYSQL,pmacctd-mysql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PMACCTD_PGSQL,pmacctd-pgsql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PMACCTD_SQLITE,pmacctd-sqlite,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PMACCTD_CUSTOM,pmacctd-custom,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PMACCT_CLIENT,pmacct-client,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(eval $(call PKG_build,PMACCT_BASIC,nfacctd-basic,pmacctd-basic,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_build,PMACCT_MYSQL,nfacctd-mysql,pmacctd-mysql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_build,PMACCT_PGSQL,nfacctd-pgsql,pmacctd-pgsql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_build,PMACCT_SQLITE,nfacctd-sqlite,pmacctd-sqlite,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_build,NFACCTD_CUSTOM,nfacctd-custom,pmacctd-dummy,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_build,PMACCTD_CUSTOM,nfacctd-dummy,pmacctd-custom,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(eval $(call PKG_package,NFACCTD_BASIC,nfacctd-basic,nfacctd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_package,NFACCTD_MYSQL,nfacctd-mysql,nfacctd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_package,NFACCTD_PGSQL,nfacctd-pgsql,nfacctd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_package,NFACCTD_SQLITE,nfacctd-sqlite,nfacctd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_package,NFACCTD_CUSTOM,nfacctd-custom,nfacctd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_package,PMACCTD_BASIC,pmacctd-basic,pmacctd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_package,PMACCTD_MYSQL,pmacctd-mysql,pmacctd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_package,PMACCTD_PGSQL,pmacctd-pgsql,pmacctd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_package,PMACCTD_SQLITE,pmacctd-sqlite,pmacctd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_package,PMACCTD_CUSTOM,pmacctd-custom,pmacctd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: $(BUILD_TARGETS)
	touch $@

$(IPKG_PMACCT_CLIENT):
	install -d -m0755 $(IDIR_PMACCT_CLIENT)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/pmacct $(IDIR_PMACCT_CLIENT)/usr/sbin/
	$(RSTRIP) $(IDIR_PMACCT_CLIENT)
	$(IPKG_BUILD) $(IDIR_PMACCT_CLIENT) $(PACKAGE_DIR)
