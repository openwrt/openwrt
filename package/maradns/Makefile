# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=maradns
PKG_VERSION:=1.0.35
PKG_RELEASE:=1
PKG_MD5SUM:=1b2ba196ddde5f545af177b4531e0d70

PKG_SOURCE_URL:=http://www.maradns.org/download
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MARADNS,maradns,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
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
		  --enable-shared \
		  --disable-static \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS)" \
		FLAGS="$(TARGET_CFLAGS)" \
		CC=$(TARGET_CC) \
		HOSTCC="$(HOSTCC)"
	mkdir -p $(PKG_INSTALL_DIR)/usr/{bin,sbin}
	mkdir -p $(PKG_INSTALL_DIR)/usr/man/man{1,5,8}
	$(MAKE) -C $(PKG_BUILD_DIR) \
		PREFIX="$(PKG_INSTALL_DIR)/usr" \
		RPM_BUILD_ROOT="$(PKG_INSTALL_DIR)" \
		install
	touch $@

$(IPKG_MARADNS):
	install -d -m0755 $(IDIR_MARADNS)/etc
	install -m0644 ./files/mararc $(IDIR_MARADNS)/etc/mararc
	install -d -m0755 $(IDIR_MARADNS)/etc/init.d
	install -m0755 ./files/maradns.init $(IDIR_MARADNS)/etc/init.d/S60maradns
	install -d -m0755 $(IDIR_MARADNS)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/{askmara,getzone} $(IDIR_MARADNS)/usr/bin/
	install -d -m0755 $(IDIR_MARADNS)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/{maradns,zoneserver} $(IDIR_MARADNS)/usr/sbin/
	$(RSTRIP) $(IDIR_MARADNS)
	$(IPKG_BUILD) $(IDIR_MARADNS) $(PACKAGE_DIR)
