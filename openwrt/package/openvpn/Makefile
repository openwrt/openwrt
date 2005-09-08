# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=openvpn
PKG_VERSION:=2.0.2
PKG_RELEASE:=1
PKG_MD5SUM:=862f8788f080f669b1ae00a74ef68001

PKG_SOURCE_URL:=http://openvpn.net/release @SF/openvpn
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,OPENVPN,openvpn,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

PKG_DEPEND:="libopenssl, kmod-tun"
ifneq ($(BR2_PACKAGE_OPENVPN_LZO),y)
DISABLE_LZO:=--disable-lzo
else
PKG_DEPEND+=", liblzo"
endif

ifneq ($(BR2_PACKAGE_OPENVPN_SERVER),y)
DISABLE_SERVER:=--disable-server
endif
ifneq ($(BR2_PACKAGE_OPENVPN_HTTP),y)
DISABLE_HTTP:=--disable-http
endif

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(strip $(TARGET_CFLAGS))" \
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
		  --disable-pthread \
		  --disable-debug \
		  --disable-plugins \
		  --disable-management \
		  --disable-socks \
		  $(DISABLE_LZO) \
		  $(DISABLE_SERVER) \
		  $(DISABLE_HTTP) \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	touch $(PKG_BUILD_DIR)/.built

$(IPKG_OPENVPN):
	install -d -m0755 $(IDIR_OPENVPN)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/openvpn $(IDIR_OPENVPN)/usr/sbin/
	$(RSTRIP) $(IDIR_OPENVPN)
	echo "Depends: $(PKG_DEPEND)" >> $(IDIR_OPENVPN)/CONTROL/control
	$(IPKG_BUILD) $(IDIR_OPENVPN) $(PACKAGE_DIR)

