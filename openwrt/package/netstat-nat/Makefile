# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=netstat-nat
PKG_VERSION:=1.4.6
PKG_RELEASE:=1
PKG_MD5SUM:=ccf71a6cbd9a513ea3adec54b54ee770

PKG_SOURCE_URL:=http://tweegy.demon.nl/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,NETSTAT-NAT,netstat-nat,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(strip $(TARGET_CFLAGS))" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
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

$(IPKG_NETSTAT-NAT):
	install -d -m0755 $(IDIR_NETSTAT-NAT)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/netstat-nat $(IDIR_NETSTAT-NAT)/usr/bin/
	$(RSTRIP) $(IDIR_NETSTAT-NAT)
	$(IPKG_BUILD) $(IDIR_NETSTAT-NAT) $(PACKAGE_DIR)

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built
