include $(TOPDIR)/rules.mk

PKG_NAME:=squid
PKG_VERSION:=2.5.STABLE13
PKG_RELEASE:=1
PKG_MD5SUM:=3e64468e123be1246d17486dab6bee87

PKG_SOURCE_URL:=http://www.squid-cache.org/Versions/v2/2.5/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=bzcat
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

AUTH_MODULES:=basic digest ntlm
ifneq ($(BR2_PACKAGE_SQUID_MOD_BASIC_AUTH_GETPWNAM),)
BASIC_AUTH_HELPERS+= getpwnam 
endif
ifneq ($(BR2_PACKAGE_SQUID_MOD_BASIC_AUTH_NCSA),)
BASIC_AUTH_HELPERS+= NCSA
endif
ifneq ($(BR2_PACKAGE_SQUID_MOD_BASIC_AUTH_SMB),)
BASIC_AUTH_HELPERS+= SMB
endif
ifneq ($(BR2_PACKAGE_SQUID_MOD_BASIC_AUTH_WINBIND),)
BASIC_AUTH_HELPERS+= winbind
endif

ifneq ($(BR2_PACKAGE_SQUID_MOD_DIGEST_AUTH_PASSWORD),)
DIGEST_AUTH_HELPERS:=password
endif

ifneq ($(BR2_PACKAGE_SQUID_MOD_EXTERNAL_ACL_IP_USER),)
EXTERNAL_ACL_HELPERS+= ip_user
endif
ifneq ($(BR2_PACKAGE_SQUID_MOD_EXTERNAL_ACL_UNIX_GROUP),)
EXTERNAL_ACL_HELPERS+= unix_group
endif
ifneq ($(BR2_PACKAGE_SQUID_MOD_EXTERNAL_ACL_WINBIND_GROUP),)
EXTERNAL_ACL_HELPERS+= winbind_group
endif

ifneq ($(BR2_PACKAGE_SQUID_MOD_NTLM_AUTH_FAKEAUTH),)
NTLM_AUTH_HELPERS+= fakeauth
endif
ifneq ($(BR2_PACKAGE_SQUID_MOD_NTLM_AUTH_SMB_AUTH),)
NTLM_AUTH_HELPERS+= SMB
endif
ifneq ($(BR2_PACKAGE_SQUID_MOD_NTLM_AUTH_WINBIND_AUTH),)
NTLM_AUTH_HELPERS+= winbind
endif

include $(TOPDIR)/package/rules.mk

define PKG_mod_template

$$(IPKG_$(1)):
	mkdir -p $$(IDIR_$(1))/usr/lib/
	[ -e $(PKG_BUILD_DIR)/helpers/$(2)/$(3)/$(4) ] && $(CP) $(PKG_BUILD_DIR)/helpers/$(2)/$(3)/$(4) $$(IDIR_$(1))/usr/lib
	$(RSTRIP) $$(IDIR_$(1))
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)

endef


$(eval $(call PKG_template,SQUID,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SQUID_MOD_BASIC_AUTH_GETPWNAM,$(PKG_NAME)-mod-basic-auth-getpwnam,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SQUID_MOD_BASIC_AUTH_NCSA,$(PKG_NAME)-mod-basic-auth-ncsa,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SQUID_MOD_BASIC_AUTH_SMB,$(PKG_NAME)-mod-basic-auth-smb,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SQUID_MOD_BASIC_AUTH_WINBIND,$(PKG_NAME)-mod-basic-auth-winbind,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SQUID_MOD_DIGEST_AUTH_PASSWORD,$(PKG_NAME)-mod-digest-auth-password,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SQUID_MOD_EXTERNAL_ACL_IP_USER,$(PKG_NAME)-mod-external-acl-ip-user,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SQUID_MOD_EXTERNAL_ACL_UNIX_GROUP,$(PKG_NAME)-mod-external-acl-unix-group,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SQUID_MOD_EXTERNAL_ACL_WINBIND_GROUP,$(PKG_NAME)-mod-external-acl-winbind-group,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SQUID_MOD_NTLM_AUTH_FAKEAUTH,$(PKG_NAME)-mod-ntlm-auth-fakeauth,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SQUID_MOD_NTLM_AUTH_SMB_AUTH,$(PKG_NAME)-mod-ntlm-auth-smb-auth,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SQUID_MOD_NTLM_AUTH_WINBIND_AUTH,$(PKG_NAME)-mod-ntlm-auth-winbind-auth,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(eval $(call PKG_mod_template,SQUID_MOD_BASIC_AUTH_GETPWNAM,basic_auth,getpwnam,getpwname_auth))
$(eval $(call PKG_mod_template,SQUID_MOD_BASIC_AUTH_LDAP,basic_auth,LDAP,ldap))
$(eval $(call PKG_mod_template,SQUID_MOD_BASIC_AUTH_NCSA,basic_auth,NCSA,ncsa_auth))
$(eval $(call PKG_mod_template,SQUID_MOD_BASIC_AUTH_SMB,basic_auth,SMB,smb_auth))
$(eval $(call PKG_mod_template,SQUID_MOD_BASIC_AUTH_WINBIND,basic_auth,winbind,wb_auth))
$(eval $(call PKG_mod_template,SQUID_MOD_DIGEST_AUTH_PASSWORD,digest_auth,password,digest_pw_auth))
$(eval $(call PKG_mod_template,SQUID_MOD_EXTERNAL_ACL_IP_USER,external_acl,ip_user,ip_user_check))
$(eval $(call PKG_mod_template,SQUID_MOD_EXTERNAL_ACL_LDAP_GROUP,external_acl,ldap_group,ldap_auth))
$(eval $(call PKG_mod_template,SQUID_MOD_EXTERNAL_ACL_UNIX_GROUP,external_acl,unix_group,squid_unix_group))
$(eval $(call PKG_mod_template,SQUID_MOD_EXTERNAL_ACL_WINBIND_GROUP,external_acl,winbind_group,wb_group))
$(eval $(call PKG_mod_template,SQUID_MOD_NTLM_AUTH_FAKEAUTH,ntlm_auth,fakeauth,fakeauth_auth))
$(eval $(call PKG_mod_template,SQUID_MOD_NTLM_AUTH_SMB_AUTH,ntlm_auth,SMB,ntlm_auth))
$(eval $(call PKG_mod_template,SQUID_MOD_NTLM_AUTH_WINBIND_AUTH,ntlm_auth,winbind,wb_ntlmauth))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		ac_cv_sizeof_void_p=4 \
		ac_cv_sizeof_short=2 \
		ac_cv_sizeof_int=4 \
		ac_cv_sizeof_long=4 \
		ac_cv_sizeof_long_long=8 \
		ac_cv_sizeof___int64=0 \
		ac_cv_sizeof_int16_t=2 \
		ac_cv_sizeof_uint16_t=2 \
		ac_cv_sizeof_u_int16_t=2 \
		ac_cv_sizeof_int32_t=4 \
		ac_cv_sizeof_uint32_t=4 \
		ac_cv_sizeof_u_int32_t=4 \
		ac_cv_sizeof_int64_t=8 \
		ac_cv_sizeof_uint64_t=8 \
		ac_cv_sizeof_u_int64_t=8 \
		ac_cv_func_setresuid=no \
		ac_cv_func_va_copy=no \
		ac_cv_func___va_copy=no \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		--with-gnu-ld \
		--enable-x-accelerator-vary \
		--with-pthreads \
		--with-dl \
		--enable-icmp \
		--enable-kill-parent-hack \
		--enable-arp-acl \
		--enable-ssl \
		--enable-htcp \
		--enable-err-languages=English \
		--enable-default-err-language=English \
		--enable-linux-netfilter \
		--enable-icmp \
		--enable-external-acl-helpers="" \
		--enable-underscores \
		--enable-cache-digests \
		--enable-referer-log \
		--enable-delay-pools \
		--enable-useragent-log \
		--with-openssl=$(STAGING_DIR)/usr \
		--enable-auth="$(AUTH_MODULES)" \
		--enable-basic-auth-helpers="$(BASIC_AUTH_HELPERS)" \
		--enable-ntlm-auth-helpers="$(NTLM_AUTH_HELPERS)" \
		--enable-digest-auth-helpers="$(DIGEST_AUTH_HELPERS)" \
		--enable-external-acl-helpers="$(EXTERNAL_ACL_HELPERS)" \
	);
	touch $@
	
$(PKG_BUILD_DIR)/.built:
	mkdir -p $(PKG_INSTALL_DIR)/usr/
	$(MAKE) DESTDIR=$(PKG_INSTALL_DIR) \
		STAGING_DIR=$(STAGING_DIR) \
		-C $(PKG_BUILD_DIR) install
	mkdir -p $(PKG_INSTALL_DIR){/etc/init.d/,/www/cgi-bin/}
	$(RSTRIP) $(PKG_INSTALL_DIR)/usr/sbin/*
	$(RSTRIP) $(PKG_INSTALL_DIR)/usr/lib/*
	(cd $(PKG_INSTALL_DIR)/usr/share/errors && ls . | grep -v English | xargs rm -rf);
	rm -r $(PKG_INSTALL_DIR){/usr/man,/etc/*.default,/usr/bin/squidclient}
	echo "" > $(PKG_INSTALL_DIR)/etc/squid.conf
	(cd $(PKG_INSTALL_DIR)/www/cgi-bin/ && rm -f cachemgr.cgi && ln -s ../../usr/lib/cachemgr.cgi cachemgr.cgi);
	touch $@

$(IPKG_SQUID):
	mkdir -p $(IDIR_SQUID)
	$(CP) $(PKG_INSTALL_DIR)/* $(IDIR_SQUID)/
	$(RSTRIP) $(IDIR_SQUID)/
	$(IPKG_BUILD) $(IDIR_SQUID) $(PACKAGE_DIR)
