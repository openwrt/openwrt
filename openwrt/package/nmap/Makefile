# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=nmap
PKG_VERSION:=4.01
PKG_RELEASE:=1
PKG_MD5SUM:=09c259837b24f6c7385c2c1c49760a7c

PKG_SOURCE_URL:=http://download.insecure.org/nmap/dist
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,NMAP,nmap,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		libpcapdir="$(STAGING_DIR)/usr/lib" \
		CFLAGS="$(TARGET_CFLAGS)" \
		CXXFLAGS="$(TARGET_CFLAGS) -fno-builtin -fno-rtti -nostdinc++" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-nodefaultlibs -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		LIBS="-luClibc++ -lc -lm -lgcc" \
		ac_cv_prog_CXX="$(TARGET_CXX)" \
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
		  --without-openssl \
		  --without-nmapfe \
		  --with-libdnet="$(STAGING_DIR)/usr" \
		  --with-libpcap="$(STAGING_DIR)/usr" \
		  --with-libpcre="$(STAGING_DIR)/usr" \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CCOPT="$(TARGET_CFLAGS) -fno-builtin -nostdinc++" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_NMAP):
	install -d -m0755 $(IDIR_NMAP)/usr/share/nmap
	for file in mac-prefixes os-fingerprints protocols rpc service-probes services; do \
		$(CP) $(PKG_INSTALL_DIR)/usr/share/nmap/nmap-$$file $(IDIR_NMAP)/usr/share/nmap; \
	done
	install -d -m0755 $(IDIR_NMAP)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nmap $(IDIR_NMAP)/usr/bin/
	$(RSTRIP) $(IDIR_NMAP)
	$(IPKG_BUILD) $(IDIR_NMAP) $(PACKAGE_DIR)

mostlyclean:
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built
