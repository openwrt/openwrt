# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=dhcp-forwarder
PKG_VERSION:=0.7
PKG_RELEASE:=4
PKG_MD5SUM:=e7f876e615ebc3f96418f6477b4451e2

PKG_SOURCE_URL:=http://savannah.nongnu.org/download/dhcp-fwd
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,DHCP_FORWARDER,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: 
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		ac_cv_func_malloc_0_nonnull="yes" \
		./configure \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --program-prefix="" \
		  --program-suffix="" \
		  --prefix=/usr \
		  --disable-dietlibc \
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
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		cfg_filename="/etc/dhcp-fwd.conf" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@
	
$(IPKG_DHCP_FORWARDER):
	install -m0755 -d $(IDIR_DHCP_FORWARDER)/etc
	install -m0644 $(PKG_BUILD_DIR)/contrib/dhcp-fwd.conf $(IDIR_DHCP_FORWARDER)/etc/
	install -m0755 -d $(IDIR_DHCP_FORWARDER)/etc/init.d
	install -m0755 ./files/dhcp-fwd.init $(IDIR_DHCP_FORWARDER)/etc/init.d/dhcp-fwd
	install -m0755 -d $(IDIR_DHCP_FORWARDER)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/dhcp-fwd $(IDIR_DHCP_FORWARDER)/usr/bin/
	$(RSTRIP) $(IDIR_DHCP_FORWARDER)
	$(IPKG_BUILD) $(IDIR_DHCP_FORWARDER) $(PACKAGE_DIR)
