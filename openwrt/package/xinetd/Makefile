# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=xinetd
PKG_VERSION:=2.3.13
PKG_RELEASE:=2
PKG_MD5SUM:=4295b5fe12350f09b5892b363348ac8b

PKG_SOURCE_URL:=http://www.xinetd.org/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,XINETD,xinetd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
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
		  --without-libwrap \
		  --with-loadavg \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		build install
	touch $@

$(IPKG_XINETD):
	install -d -m0755 $(IDIR_XINETD)/etc
	install -m0644 ./files/xinetd.conf $(IDIR_XINETD)/etc/xinetd.conf
	install -d -m0755 $(IDIR_XINETD)/etc/xinetd.d
	install -d -m0755 $(IDIR_XINETD)/etc/init.d
	install -m0755 ./files/xinetd.init $(IDIR_XINETD)/etc/init.d/xinetd
	install -d -m0755 $(IDIR_XINETD)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/xinetd $(IDIR_XINETD)/usr/sbin/
	$(RSTRIP) $(IDIR_XINETD)
	$(IPKG_BUILD) $(IDIR_XINETD) $(PACKAGE_DIR)
