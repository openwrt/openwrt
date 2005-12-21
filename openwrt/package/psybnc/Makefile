# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=psybnc
PKG_VERSION:=2.3.2-7
PKG_RELEASE:=1
PKG_MD5SUM:=4b893a92f914813450e0921b2894ad6c

PKG_SOURCE_URL:=http://www.psybnc.info/download/beta/
PKG_SOURCE:=psyBNC-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PSYBNC,psybnc,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		HOSTCC="$(HOSTCC)" \
		all
	touch $@

$(IPKG_PSYBNC): 
	install -d -m0755 $(IDIR_PSYBNC)/usr/share/psybnc/lang/
	install -d -m0755 $(IDIR_PSYBNC)/usr/share/psybnc/log/
	install -d -m0755 $(IDIR_PSYBNC)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/psybnc $(IDIR_PSYBNC)/usr/share/psybnc/
	install -m0600 $(PKG_BUILD_DIR)/psybnc.conf $(IDIR_PSYBNC)/usr/share/psybnc/
	install -m0644 $(PKG_BUILD_DIR)/lang/english.lng $(IDIR_PSYBNC)/usr/share/psybnc/lang/
	install -m0755 $(PKG_BUILD_DIR)/psybnc.sh $(IDIR_PSYBNC)/usr/sbin/psybnc
	$(RSTRIP) $(IDIR_PSYBNC)
	$(IPKG_BUILD) $(IDIR_PSYBNC) $(PACKAGE_DIR)

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
