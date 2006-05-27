# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=freeradius
PKG_VERSION:=1.1.1
PKG_RELEASE:=1
PKG_MD5SUM:=b38b24f6352090fdb571b9e8da52e12e

PKG_SOURCE_URL:=ftp://ftp.freeradius.org/pub/radius/ \
	http://freeradius.portal-to-web.de/ \
	ftp://ftp.uk.freeradius.org/pub/radius/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

PKG_CONFIGURE_OPTIONS := \
	--enable-shared \
	--disable-static \
	--disable-ltdl-install \
	--with-ltdl-include="$(STAGING_DIR)/usr/include" \
	--with-ltdl-lib="$(STAGING_DIR)/usr/lib" \
	--with-openssl-includes="$(STAGING_DIR)/usr/include" \
	--with-openssl-libraries="$(STAGING_DIR)/usr/lib" \
	--enable-strict-dependencies \
	--with-raddbdir=/etc/freeradius \
	--without-edir \
	--without-snmp \
	--with-experimental-modules \
	--without-rlm_attr-rewrite \
	--without-rlm_checkval \
	--without-rlm_counter \
	--without-rlm_dbm \
	--with-rlm_eap \
	--without-rlm_eap_sim \
	--without-rlm_example \
	--without-rlm_ippool \
	--without-rlm_krb5 \
	--without-rlm_otp \
	--without-rlm_pam \
	--without-rlm_perl \
	--without-rlm_python \
	--without-rlm_radutmp \
	--without-rlm_smb \
	--with-rlm_sql \
	--with-rlm_sqlcounter \
	--without-rlm_sql_db2 \
	--without-rlm_sql_freetds \
	--without-rlm_sql_iodbc \
	--without-rlm_sql_oracle \
	--without-rlm_sql_sybase \
	--without-rlm_sql_unixodbc \
	--without-rlm_sql_log \
	--without-rlm_unix \

ifneq ($(BR2_PACKAGE_FREERADIUS_MOD_LDAP),)
PKG_CONFIGURE_LIBS += -lcrypto -lssl
PKG_CONFIGURE_OPTIONS += \
	--with-rlm_ldap-include-dir="$(STAGING_DIR)/usr/include" \
	--with-rlm_ldap-lib-dir="$(STAGING_DIR)/usr/lib"
else
PKG_CONFIGURE_OPTIONS += --without-rlm_ldap
endif
ifneq ($(BR2_PACKAGE_FREERADIUS_MOD_SQL_MYSQL),)
PKG_CONFIGURE_LIBS += -lz
PKG_CONFIGURE_OPTIONS += \
	--with-mysql-include-dir="$(STAGING_DIR)/usr/include" \
	--with-mysql-lib-dir="$(STAGING_DIR)/usr/lib/mysql" \
	--without-threads
else
PKG_CONFIGURE_OPTIONS += --without-rlm_sql_mysql
endif
ifneq ($(BR2_PACKAGE_FREERADIUS_MOD_SQL_PGSQL),)
PKG_CONFIGURE_OPTIONS += \
	--with-rlm_sql_postgresql-include-dir="$(STAGING_DIR)/usr/include" \
	--with-rlm_sql_postgresql-lib-dir="$(STAGING_DIR)/usr/lib"
else
PKG_CONFIGURE_OPTIONS += --without-rlm_sql_postgresql
endif

include $(TOPDIR)/package/rules.mk

define PKG_mod_template

$$(IPKG_$(1)):
	[ -z "$(2)" ] || install -d -m0755 $$(IDIR_$(1))/usr/lib/freeradius
	for m in $(2); do \
		$(CP) $(PKG_INSTALL_DIR)/usr/lib/freeradius/$$$${m}{,-*}.so \
		  $$(IDIR_$(1))/usr/lib/freeradius/ ; \
	done
	[ -z "$(3)" ] || install -d -m0755 $$(IDIR_$(1))/etc/freeradius
	for f in $(3); do \
		$(CP) $(PKG_INSTALL_DIR)/etc/freeradius/$$$${f} \
		  $$(IDIR_$(1))/etc/freeradius/ ; \
	done
	$(RSTRIP) $$(IDIR_$(1))
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)

endef

$(eval $(call PKG_template,FREERADIUS,freeradius,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_DEMOCERTS,freeradius-democerts,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_CHAP,freeradius-mod-chap,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_DETAIL,freeradius-mod-detail,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_EAP,freeradius-mod-eap,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_EAP_GTC,freeradius-mod-eap-gtc,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_EAP_MD5,freeradius-mod-eap-md5,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_EAP_MSCHAPV2,freeradius-mod-eap-mschapv2,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_EAP_PEAP,freeradius-mod-eap-peap,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_EAP_TLS,freeradius-mod-eap-tls,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_EAP_TTLS,freeradius-mod-eap-ttls,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_FILES,freeradius-mod-files,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_LDAP,freeradius-mod-ldap,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_MSCHAP,freeradius-mod-mschap,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_PAP,freeradius-mod-pap,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_PREPROCESS,freeradius-mod-preprocess,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_REALM,freeradius-mod-realm,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_SQL,freeradius-mod-sql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_SQL_MYSQL,freeradius-mod-sql-mysql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_SQL_PGSQL,freeradius-mod-sql-pgsql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_MOD_SQLCOUNTER,freeradius-mod-sqlcounter,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FREERADIUS_UTILS,freeradius-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(eval $(call PKG_mod_template,FREERADIUS_MOD_CHAP,rlm_chap,))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_DETAIL,rlm_detail,))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_EAP,libeap rlm_eap,eap.conf))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_EAP_GTC,rlm_eap_gtc,))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_EAP_MD5,rlm_eap_md5,))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_EAP_MSCHAPV2,rlm_eap_mschapv2,))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_EAP_PEAP,rlm_eap_peap,))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_EAP_TLS,rlm_eap_tls,))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_EAP_TTLS,rlm_eap_ttls,))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_FILES,rlm_files,acct_users preproxy_users users))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_LDAP,rlm_ldap,ldap.attrmap))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_MSCHAP,rlm_mschap,))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_PAP,rlm_pap,))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_PREPROCESS,rlm_preprocess,hints huntgroups))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_REALM,rlm_realm,proxy.conf))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_SQL,rlm_sql,sql.conf))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_SQL_MYSQL,rlm_sql_mysql,))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_SQL_PGSQL,rlm_sql_postgresql,))
$(eval $(call PKG_mod_template,FREERADIUS_MOD_SQLCOUNTER,rlm_sqlcounter,))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib -L$(PKG_INSTALL_DIR)/usr/lib/freeradius" \
		LIBS="$(PKG_CONFIGURE_LIBS)" \
		sys_lib_search_path_spec="$(STAGING_DIR)/usr/lib $(STAGING_DIR)/lib" \
		MYSQL_CONFIG="no" \
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
			--libdir=/usr/lib/freeradius \
			--libexecdir=/usr/lib/freeradius \
			--localstatedir=/var \
			--mandir=/usr/share/man \
			--sbindir=/usr/sbin \
			--sysconfdir=/etc \
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
			$(PKG_CONFIGURE_OPTIONS) \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		R="$(PKG_INSTALL_DIR)" \
		INSTALLSTRIP="" \
		all install
	touch $@

$(IPKG_FREERADIUS):
	install -m0755 -d $(IDIR_FREERADIUS)/etc/init.d
	install -m0755 ./files/radiusd.init $(IDIR_FREERADIUS)/etc/init.d/radiusd
	install -m0755 -d $(IDIR_FREERADIUS)/etc/freeradius
	for f in clients.conf dictionary radiusd.conf; do \
		$(CP) $(PKG_INSTALL_DIR)/etc/freeradius/$${f} \
		  $(IDIR_FREERADIUS)/etc/freeradius/ ; \
	done
	install -m0755 -d $(IDIR_FREERADIUS)/usr/share/freeradius
	$(CP) $(PKG_INSTALL_DIR)/usr/share/freeradius/dictionary \
		$(IDIR_FREERADIUS)/usr/share/freeradius/
	for f in freeradius freeradius.internal rfc2865 rfc2866 rfc2867 rfc2868 rfc2869 rfc3162 rfc3576 rfc3580 microsoft wispr; do \
		$(CP) $(PKG_INSTALL_DIR)/usr/share/freeradius/dictionary.$${f} \
		  $(IDIR_FREERADIUS)/usr/share/freeradius/ ; \
	done
	install -m0755 -d $(IDIR_FREERADIUS)/usr/lib/freeradius
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/freeradius/libradius{,-*}.so \
	  $(IDIR_FREERADIUS)/usr/lib/freeradius/
	install -m0755 -d $(IDIR_FREERADIUS)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/radiusd \
	  $(IDIR_FREERADIUS)/usr/sbin/
	$(RSTRIP) $(IDIR_FREERADIUS)
	$(IPKG_BUILD) $(IDIR_FREERADIUS) $(PACKAGE_DIR)

$(IPKG_FREERADIUS_DEMOCERTS):
	install -m0755 -d $(IDIR_FREERADIUS_DEMOCERTS)/etc/freeradius
	$(CP) $(PKG_INSTALL_DIR)/etc/freeradius/certs \
	  $(IDIR_FREERADIUS_DEMOCERTS)/etc/freeradius/
	rm -rf $(IDIR_FREERADIUS_DEMOCERTS)/etc/freeradius/certs/README
	rm -rf $(IDIR_FREERADIUS_DEMOCERTS)/etc/freeradius/certs/new*
	rm -rf $(IDIR_FREERADIUS_DEMOCERTS)/etc/freeradius/certs/demoCA/index*
	rm -rf $(IDIR_FREERADIUS_DEMOCERTS)/etc/freeradius/certs/demoCA/serial*
	$(RSTRIP) $(IDIR_FREERADIUS_DEMOCERTS)
	$(IPKG_BUILD) $(IDIR_FREERADIUS_DEMOCERTS) $(PACKAGE_DIR)

$(IPKG_FREERADIUS_UTILS):
	install -m0755 -d $(IDIR_FREERADIUS_UTILS)/usr/bin
	for f in radclient; do \
	  $(CP) $(PKG_INSTALL_DIR)/usr/bin/$${f} \
	    $(IDIR_FREERADIUS_UTILS)/usr/bin/ ; \
	done
	$(RSTRIP) $(IDIR_FREERADIUS_UTILS)
	$(IPKG_BUILD) $(IDIR_FREERADIUS_UTILS) $(PACKAGE_DIR)
	
