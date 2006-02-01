# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=updatedd
PKG_VERSION:=2.5
PKG_RELEASE:=1
PKG_MD5SUM:=2957496de0c8e08e9c6492fabf1a88be

PKG_SOURCE_URL:=http://savannah.nongnu.org/download/updatedd/
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

define PKG_mod_template

$$(IPKG_$(1)):
	install -d -m0755 $$(IDIR_$(1))/usr/lib/updatedd
	install -m0755 $(PKG_INSTALL_DIR)/usr/lib/updatedd/$(2).so $$(IDIR_$(1))/usr/lib/updatedd/
	$(RSTRIP) $$(IDIR_$(1))
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)

endef

$(eval $(call PKG_template,UPDATEDD,updatedd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,UPDATEDD_MOD_CHANGEIP,updatedd-mod-changeip,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,UPDATEDD_MOD_DYNDNS,updatedd-mod-dyndns,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,UPDATEDD_MOD_EURODYNDNS,updatedd-mod-eurodyndns,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,UPDATEDD_MOD_HN,updatedd-mod-hn,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,UPDATEDD_MOD_NOIP,updatedd-mod-noip,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,UPDATEDD_MOD_ODS,updatedd-mod-ods,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,UPDATEDD_MOD_OVH,updatedd-mod-ovh,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,UPDATEDD_MOD_REGFISH,updatedd-mod-regfish,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,UPDATEDD_MOD_TZO,updatedd-mod-tzo,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(eval $(call PKG_mod_template,UPDATEDD_MOD_CHANGEIP,changeip))
$(eval $(call PKG_mod_template,UPDATEDD_MOD_DYNDNS,dyndns))
$(eval $(call PKG_mod_template,UPDATEDD_MOD_EURODYNDNS,eurodyndns))
$(eval $(call PKG_mod_template,UPDATEDD_MOD_HN,hn))
$(eval $(call PKG_mod_template,UPDATEDD_MOD_NOIP,noip))
$(eval $(call PKG_mod_template,UPDATEDD_MOD_ODS,ods))
$(eval $(call PKG_mod_template,UPDATEDD_MOD_OVH,ovh))
$(eval $(call PKG_mod_template,UPDATEDD_MOD_REGFISH,regfish))
$(eval $(call PKG_mod_template,UPDATEDD_MOD_TZO,tzo))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--prefix=/usr \
			--sysconfdir=/etc \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_UPDATEDD):
	install -d -m0755 $(IDIR_UPDATEDD)/etc/init.d
	install -m0755 ./files/S55ddns $(IDIR_UPDATEDD)/etc/init.d/
	install -d -m0755 $(IDIR_UPDATEDD)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/updatedd $(IDIR_UPDATEDD)/usr/bin/
	install -d -m0755 $(IDIR_UPDATEDD)/usr/lib/updatedd
	$(RSTRIP) $(IDIR_UPDATEDD)
	$(IPKG_BUILD) $(IDIR_UPDATEDD) $(PACKAGE_DIR)
