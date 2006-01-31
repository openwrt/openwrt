# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=openser
PKG_VERSION:=1.0.0
PKG_RELEASE:=1
PKG_MD5SUM:=f60f5a51772c5b06abaa1c81e9539077
# PKG_VARIANT:=-tls for tls version
PKG_VARIANT:=

PKG_SOURCE_URL:=http://openser.org/pub/openser/$(PKG_VERSION)/src
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)$(PKG_VARIANT)_src.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,OPENSER,openser,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
	cp -fpR $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_OPENSER)/usr/sbin/
	cp -fpR $(PKG_BUILD_DIR)/utils/gen_ha1/gen_ha1 $(IDIR_OPENSER)/usr/sbin/openser_gen_ha1
	cp -fpR $(PKG_BUILD_DIR)/scripts/sc $(IDIR_OPENSER)/usr/sbin/openserctl
	chmod 744 $(IDIR_OPENSER)/usr/sbin/openserctl
	cp -fpR $(PKG_BUILD_DIR)/scripts/sc.dbtext $(IDIR_OPENSER)/usr/sbin/dbtextctl
	chmod 744 $(IDIR_OPENSER)/usr/sbin/dbtextctl
	mkdir -p $(IDIR_OPENSER)/usr/lib/openser/modules
	(cd $(PKG_BUILD_DIR);\
		cp -a $(OPENSER_MODULE_FILES) $(IDIR_OPENSER)/usr/lib/openser/modules/; \
	)
	mkdir -p $(IDIR_OPENSER)/etc/openser
	cp -fpR $(PKG_BUILD_DIR)/etc/openser.cfg $(IDIR_OPENSER)/etc/openser/
	$(RSTRIP) $(IDIR_OPENSER)
	$(IPKG_BUILD) $(IDIR_OPENSER) $(PACKAGE_DIR)
