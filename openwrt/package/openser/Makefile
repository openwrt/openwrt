# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=openser
PKG_VERSION:=1.0.1
PKG_RELEASE:=1
PKG_MD5SUM:=eebc16665003ee92daf96b216d6f9563
# PKG_VARIANT:=-tls # for tls version
PKG_VARIANT:=

PKG_SOURCE_URL:=http://openser.org/pub/openser/$(PKG_VERSION)/src
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)$(PKG_VARIANT)_src.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

define PKG_mod_template

$$(IPKG_$(1)):
	[ -z "$(2)" ] || install -d -m 0755 $$(IDIR_$(1))/usr/lib/openser/modules/
	$(CP) $(PKG_BUILD_DIR)/modules/$(2)/$(2).so \
			$$(IDIR_$(1))/usr/lib/openser/modules/ ; \
	$(RSTRIP) $$(IDIR_$(1))
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)
	
endef

$(eval $(call PKG_template,OPENSER,openser,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_ACCOUNTING,openser-mod-acc,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_AUTH,openser-mod-auth,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_AUTH_DB,openser-mod-auth-db,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_AUTH_RADIUS,openser-mod-auth-radius,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_AVP_RADIUS,openser-mod-avp-radius,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_AVPOPS,openser-mod-avpops,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_DISPATCHER,openser-mod-dispatcher,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_DIVERSION,openser-mod-diversion,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_FLATSTORE,openser-mod-flatstore,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_GFLAGS,openser-mod-gflags,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_GROUP,openser-mod-group,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_GROUP_RADIUS,openser-mod-group-radius,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_LCR,openser-mod-lcr,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_MEDIAPROXY,openser-mod-mediaproxy,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_MSILO,openser-mod-msilo,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_MYSQL,openser-mod-mysql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_NATHELPER,openser-mod-nathelper,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_OPTIONS,openser-mod-options,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_PDT,openser-mod-pdt,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_PERMISSIONS,openser-mod-permissions,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_PIKE,openser-mod-pike,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_SMS,openser-mod-sms,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_UAC,openser-mod-uac,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_UAC_REDIRECT,openser-mod-uac-redirect,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_URI,openser-mod-uri,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_URI_DB,openser-mod-uri-db,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_URI_RADIUS,openser-mod-uri-radius,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSER_MOD_XLOG,openser-mod-xlog,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(eval $(call PKG_mod_template,OPENSER_MOD_ACCOUNTING,acc))
$(eval $(call PKG_mod_template,OPENSER_MOD_AUTH,auth))
$(eval $(call PKG_mod_template,OPENSER_MOD_AUTH_DB,auth_db))
$(eval $(call PKG_mod_template,OPENSER_MOD_AUTH_RADIUS,auth_radius))
$(eval $(call PKG_mod_template,OPENSER_MOD_AVP_RADIUS,avp_radius))
$(eval $(call PKG_mod_template,OPENSER_MOD_AVPOPS,avpops))
$(eval $(call PKG_mod_template,OPENSER_MOD_DISPATCHER,dispatcher))
$(eval $(call PKG_mod_template,OPENSER_MOD_DIVERSION,diversion))
$(eval $(call PKG_mod_template,OPENSER_MOD_FLATSTORE,flatstore))
$(eval $(call PKG_mod_template,OPENSER_MOD_GFLAGS,gflags))
$(eval $(call PKG_mod_template,OPENSER_MOD_GROUP,group))
$(eval $(call PKG_mod_template,OPENSER_MOD_GROUP_RADIUS,group_radius))
$(eval $(call PKG_mod_template,OPENSER_MOD_LCR,lcr))
$(eval $(call PKG_mod_template,OPENSER_MOD_MEDIAPROXY,mediaproxy))
$(eval $(call PKG_mod_template,OPENSER_MOD_MSILO,msilo))
$(eval $(call PKG_mod_template,OPENSER_MOD_MYSQL,mysql))
$(eval $(call PKG_mod_template,OPENSER_MOD_NATHELPER,nathelper))
$(eval $(call PKG_mod_template,OPENSER_MOD_OPTIONS,options))
$(eval $(call PKG_mod_template,OPENSER_MOD_PDT,pdt))
$(eval $(call PKG_mod_template,OPENSER_MOD_PERMISSIONS,permissions))
$(eval $(call PKG_mod_template,OPENSER_MOD_PIKE,pike))
$(eval $(call PKG_mod_template,OPENSER_MOD_SMS,sms))
$(eval $(call PKG_mod_template,OPENSER_MOD_SPEEDIAL,speeddial))
$(eval $(call PKG_mod_template,OPENSER_MOD_UAC,uac))
$(eval $(call PKG_mod_template,OPENSER_MOD_UAC_REDIRECT,uac_redirect))
$(eval $(call PKG_mod_template,OPENSER_MOD_URI,uri))
$(eval $(call PKG_mod_template,OPENSER_MOD_URI_DB,uri_db))
$(eval $(call PKG_mod_template,OPENSER_MOD_URI_RADIUS,uri_radius))
$(eval $(call PKG_mod_template,OPENSER_MOD_XLOG,xlog))

# Select here the modules for the ipk package
OPENSER_MODULES := sl tm rr maxfwd usrloc registrar dbtext textops exec

OPENSER_MODULE_FILES := $(foreach module,$(OPENSER_MODULES),modules/$(module)/$(module).so)
OPENSER_MODULES := $(patsubst %,modules/%,$(OPENSER_MODULES))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		prefix=/ \
		extra_defs="-DUSE_PTHREAD_MUTEX "  \
		CC="$(TARGET_CC)" \
		ARCH="$(ARCH)"  \
		CFLAGS="$(TARGET_CFLAGS)" \
		LOCALBASE="$(STAGING_DIR)/usr" \
		all utils/gen_ha1/gen_ha1
	touch $@

$(IPKG_OPENSER): 
	mkdir -p $(IDIR_OPENSER)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_OPENSER)/usr/sbin/
	$(CP) $(PKG_BUILD_DIR)/utils/gen_ha1/gen_ha1 $(IDIR_OPENSER)/usr/sbin/openser_gen_ha1
	$(CP) $(PKG_BUILD_DIR)/scripts/sc $(IDIR_OPENSER)/usr/sbin/openserctl
	chmod 744 $(IDIR_OPENSER)/usr/sbin/openserctl
	$(CP) $(PKG_BUILD_DIR)/scripts/sc.dbtext $(IDIR_OPENSER)/usr/sbin/dbtextctl
	chmod 744 $(IDIR_OPENSER)/usr/sbin/dbtextctl
	mkdir -p $(IDIR_OPENSER)/usr/lib/openser/modules
	(cd $(PKG_BUILD_DIR);\
		$(CP) $(OPENSER_MODULE_FILES) $(IDIR_OPENSER)/usr/lib/openser/modules/; \
	)
	mkdir -p $(IDIR_OPENSER)/etc/openser
	$(CP) $(PKG_BUILD_DIR)/etc/openser.cfg $(IDIR_OPENSER)/etc/openser/
	$(RSTRIP) $(IDIR_OPENSER)
	$(IPKG_BUILD) $(IDIR_OPENSER) $(PACKAGE_DIR)
