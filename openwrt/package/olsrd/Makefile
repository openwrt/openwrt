# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=olsrd
PKG_VERSION:=0.4.10
PKG_RELEASE:=1
PKG_MD5SUM:=9807d4451e65cb4ec385155eef7bf3cf

PKG_SOURCE_URL:=http://www.olsr.org/releases/0.4
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

define PKG_mod_template

$$(IPKG_$(1)):
	install -d -m0755 $$(IDIR_$(1))/usr/lib
	install -m0755 $(PKG_INSTALL_DIR)/usr/lib/olsrd_$(2).so.* $$(IDIR_$(1))/usr/lib/
	$(RSTRIP) $$(IDIR_$(1))
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)

endef

$(eval $(call PKG_template,OLSRD,olsrd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OLSRD_MOD_DOT_DRAW,olsrd-mod-dot-draw,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OLSRD_MOD_DYN_GW,olsrd-mod-dyn-gw,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OLSRD_MOD_HTTPINFO,olsrd-mod-httpinfo,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OLSRD_MOD_NAMESERVICE,olsrd-mod-nameservice,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OLSRD_MOD_POWER,olsrd-mod-power,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OLSRD_MOD_SECURE,olsrd-mod-secure,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OLSRD_MOD_TAS,olsrd-mod-tas,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(eval $(call PKG_mod_template,OLSRD_MOD_DOT_DRAW,dot_draw))
$(eval $(call PKG_mod_template,OLSRD_MOD_DYN_GW,dyn_gw))
$(eval $(call PKG_mod_template,OLSRD_MOD_HTTPINFO,httpinfo))
$(eval $(call PKG_mod_template,OLSRD_MOD_NAMESERVICE,nameservice))
$(eval $(call PKG_mod_template,OLSRD_MOD_POWER,power))
$(eval $(call PKG_mod_template,OLSRD_MOD_SECURE,secure))
$(eval $(call PKG_mod_template,OLSRD_MOD_TAS,tas))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		$(TARGET_CONFIGURE_OPTS) \
		NODEBUG=1 \
		OFLAGS="$(TARGET_CFLAGS)" \
		OS="linux" \
		INSTALL_PREFIX="$(PKG_INSTALL_DIR)" \
		STRIP="/bin/true" \
		all libs install install_libs
	touch $@

$(IPKG_OLSRD):
	install -d -m0755 $(IDIR_OLSRD)/etc
	cp -fpR $(PKG_INSTALL_DIR)/etc/olsrd.conf $(IDIR_OLSRD)/etc/
	install -d -m0755 $(IDIR_OLSRD)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/olsrd $(IDIR_OLSRD)/usr/sbin/
	install -d -m0755 $(IDIR_OLSRD)/etc/init.d
	install -m0755 ./files/olsrd.init $(IDIR_OLSRD)/etc/init.d/S60olsrd
	$(RSTRIP) $(IDIR_OLSRD)
	$(IPKG_BUILD) $(IDIR_OLSRD) $(PACKAGE_DIR)

$(IPKG_OLSRD_MOD_SECURE): $(IDIR_OLSRD_MOD_SECURE)/etc/olsrd.d/olsrd_secure_key

$(IDIR_OLSRD_MOD_SECURE)/etc/olsrd.d/olsrd_secure_key:
	install -d -m0755 $(IDIR_OLSRD_MOD_SECURE)/etc/olsrd.d
	cp -fpR ./files/olsrd_secure_key $(IDIR_OLSRD_MOD_SECURE)/etc/olsrd.d/
