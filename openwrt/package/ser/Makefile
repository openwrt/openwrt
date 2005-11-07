# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ser
PKG_VERSION:=0.8.14
PKG_RELEASE:=1
PKG_MD5SUM:=3e2e12c8dfbd7dad9199304093de0838

PKG_SOURCE_URL:=ftp://ftp.berlios.de/pub/ser/$(PKG_VERSION)/src
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)_src.tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,SER,ser,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

SER_MODULES := sl tm rr maxfwd usrloc registrar dbtext textops nathelper

SER_MODULE_FILES := $(foreach module,$(SER_MODULES),modules/$(module)/$(module).so)
SER_MODULES := $(patsubst %,modules/%,$(SER_MODULES))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		extra_defs="-DUSE_PTHREAD_MUTEX "  \
		CC="$(TARGET_CC)" \
		ARCH="$(ARCH)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		modules all
	touch $@

$(IPKG_SER):
	mkdir -p $(IDIR_SER)/usr/sbin
	cp -a $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_SER)/usr/sbin/
	-$(STRIP) $(IDIR_SER)/usr/sbin/*
	cp -a $(PKG_BUILD_DIR)/scripts/sc $(IDIR_SER)/usr/sbin/serctl
	mkdir -p $(IDIR_SER)/usr/lib/ser/modules
	(cd $(PKG_BUILD_DIR);\
		cp -a $(SER_MODULE_FILES) $(IDIR_SER)/usr/lib/ser/modules/; \
	)
	$(STRIP) $(IDIR_SER)/usr/lib/ser/modules/*
	mkdir -p $(IDIR_SER)/etc/ser
	cp -a $(PKG_BUILD_DIR)/etc/ser.cfg $(IDIR_SER)/etc/ser/
	$(IPKG_BUILD) $(IDIR_SER) $(PACKAGE_DIR)

