# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=rarpd
PKG_VERSION:=1.1
PKG_RELEASE:=1
PKG_MD5SUM:=04e2ca849e758d0b88c8281775ec3b58

PKG_SOURCE_URL:=ftp://ftp.dementia.org/pub/net-tools
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

# Hack Alert!
# The configure script for rarpd, when used with the --with-libnet option
# doesn't correctly detect and set -DNEW_LIBNET_INTERFACE.
EXTRA_CFLAGS = -DNEW_LIBNET_INTERFACE

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,RARPD,rarpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-I$(STAGING_DIR)/usr/lib -I$(STAGING_DIR)/lib" \
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
			--with-libnet="$(STAGING_DIR)/usr" \
			--with-pcap="$(STAGING_DIR)/usr" \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_RARPD):
	install -d -m0755 $(IDIR_RARPD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/rarpd $(IDIR_RARPD)/usr/sbin/
	install -d -m0755 $(IDIR_RARPD)/etc/default
	install -m644 ./files/rarpd.default $(IDIR_RARPD)/etc/default/rarpd
	install -d -m0755 $(IDIR_RARPD)/etc/init.d
	install -m755 ./files/rarpd.init $(IDIR_RARPD)/etc/init.d/rarpd
	ln -sf rarpd $(IDIR_RARPD)/etc/init.d/S50rarpd
	$(RSTRIP) $(IDIR_RARPD)
	$(IPKG_BUILD) $(IDIR_RARPD) $(PACKAGE_DIR)

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
