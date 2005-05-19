# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=monit
PKG_VERSION:=4.5.1
PKG_RELEASE:=1
PKG_MD5SUM:=0f054ac39822b4be71789d49e4813754

PKG_SOURCE_URL:=http://www.tildeslash.com/monit/dist/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MONIT,monit,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --program-prefix="" \
		  --program-suffix="" \
		  --prefix=/usr \
		  --exec-prefix=/usr \
		  --bindir=/usr/sbin \
		  --datadir=/usr/share \
		  --includedir=/usr/include \
		  --infodir=/usr/share/info \
		  --libdir=/usr/lib \
		  --libexecdir=/usr/lib \
		  --localstatedir=/var \
		  --mandir=/usr/share/man \
		  --sbindir=/usr/sbin \
		  --sysconfdir=/etc \
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
		  --without-ssl \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)
	touch $@

$(IPKG_MONIT):
	install -d -m0755 $(IDIR_MONIT)/etc
	install -m0600 $(PKG_BUILD_DIR)/monitrc $(IDIR_MONIT)/etc/
	install -d -m0755 $(IDIR_MONIT)/etc/init.d
	install -m0755 ./files/monit.init $(IDIR_MONIT)/etc/init.d/monit
	install -d -m0755 $(IDIR_MONIT)/usr/sbin
	cp -fpR $(PKG_BUILD_DIR)/monit $(IDIR_MONIT)/usr/sbin/
	$(STRIP) $(IDIR_MONIT)/usr/sbin/monit
	$(IPKG_BUILD) $(IDIR_MONIT) $(PACKAGE_DIR)
