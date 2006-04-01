# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ez-ipupdate
PKG_VERSION:=3.0.11b8
PKG_RELEASE:=2
PKG_MD5SUM:=000211add4c4845ffa4211841bff4fb0

PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/e/ez-ipupdate \
	http://ftp.de.debian.org/debian/pool/main/e/ez-ipupdate
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,EZIPUPDATE,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--datadir=/usr/share \
		--includedir=/usr/include \
		--infodir=/usr/share/info \
		--libdir=/usr/lib \
		--libexecdir=/usr/lib/locate \
		--localstatedir=/var/lib \
		--mandir=/usr/share/man \
		--sbindir=/usr/sbin \
		--sysconfdir=/etc \
		$(DISABLE_NLS) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)
	touch $@

$(IPKG_EZIPUPDATE):
	install -d -m0755 $(IDIR_EZIPUPDATE)/etc
	install -m0600 ./files/$(PKG_NAME).conf $(IDIR_EZIPUPDATE)/etc/
	install -d -m0755 $(IDIR_EZIPUPDATE)/etc/hotplug.d/iface
	install -m0755 ./files/$(PKG_NAME).hotplug $(IDIR_EZIPUPDATE)/etc/hotplug.d/iface/10-$(PKG_NAME)
	install -d -m0755 $(IDIR_EZIPUPDATE)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_EZIPUPDATE)/usr/sbin/
	$(RSTRIP) $(IDIR_EZIPUPDATE)
	$(IPKG_BUILD) $(IDIR_EZIPUPDATE) $(PACKAGE_DIR)

