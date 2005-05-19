# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=olsrd
PKG_VERSION:=0.4.9
PKG_RELEASE:=1
PKG_MD5SUM:=593c0861fa10d2a8d0e7d8617479c5cf

PKG_SOURCE_URL:=http://www.olsr.org/releases/0.4
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

define PKG_mod_template

$$(IPKG_$(1)):
	install -d -m0755 $$(IDIR_$(1))/usr/lib
	install -m0755 $(PKG_BUILD_DIR)/lib/$(2)*/olsrd_$(2).so.* $$(IDIR_$(1))/usr/lib/
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

$(eval $(call PKG_mod_template,OLSRD_MOD_DOT_DRAW,dot_draw))
$(eval $(call PKG_mod_template,OLSRD_MOD_DYN_GW,dyn_gw))
$(eval $(call PKG_mod_template,OLSRD_MOD_HTTPINFO,httpinfo))
$(eval $(call PKG_mod_template,OLSRD_MOD_NAMESERVICE,nameservice))
$(eval $(call PKG_mod_template,OLSRD_MOD_POWER,power))
$(eval $(call PKG_mod_template,OLSRD_MOD_SECURE,secure))

$(PKG_BUILD_DIR)/.configured:
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		OS="linux" \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -DOLSR_PLUGIN -I. -Isrc -Dlinux -I../../src -DNODEBUG" \
		libs
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		OS="linux" \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -I. -Isrc -Dlinux -I../../src -DNODEBUG" \
		all
	touch $(PKG_BUILD_DIR)/.built

$(IPKG_OLSRD):
	install -d -m0755 $(IDIR_OLSRD)/etc
	cp -fpR $(PKG_BUILD_DIR)/files/olsrd.conf.default.rfc $(IDIR_OLSRD)/etc/olsrd.conf
	install -d -m0755 $(IDIR_OLSRD)/usr/sbin
	cp -fpR $(PKG_BUILD_DIR)/olsrd $(IDIR_OLSRD)/usr/sbin/
	install -d -m0755 $(IDIR_OLSRD)/etc/init.d
	cp -fpR ./files/olsrd.init $(IDIR_OLSRD)/etc/init.d/olsrd
	$(RSTRIP) $(IDIR_OLSRD)
	$(IPKG_BUILD) $(IDIR_OLSRD) $(PACKAGE_DIR)

$(IPKG_OLSRD_MOD_SECURE): $(IDIR_OLSRD_MOD_SECURE)/etc/olsrd.d/olsrd_secure_key

$(IDIR_OLSRD_MOD_SECURE)/etc/olsrd.d/olsrd_secure_key:
	install -d -m0755 $(IDIR_OLSRD_MOD_SECURE)/etc/olsrd.d
	cp -fpR ./files/olsrd_secure_key $(IDIR_OLSRD_MOD_SECURE)/etc/olsrd.d/
