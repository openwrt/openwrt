# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=privoxy
PKG_VERSION:=3.0.3
PKG_RELEASE:=1
PKG_MD5SUM:=b0710be8af09409bfb03f52523a17010

PKG_SOURCE_URL:=@SF/ijbswa
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-stable-src.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)-stable
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PRIVOXY,privoxy,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		autoheader && autoconf && \
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
		  $(DISABLE_NLS) \
		  --disable-static \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		SPECIAL_CFLAGS="-pthread -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib"
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	touch $(PKG_BUILD_DIR)/.built

$(IPKG_PRIVOXY):
	install -d -m0755 $(IDIR_PRIVOXY)/usr/sbin $(IDIR_PRIVOXY)/etc/privoxy 
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/privoxy $(IDIR_PRIVOXY)/usr/sbin/
	$(RSTRIP) $(IDIR_PRIVOXY)
	$(IPKG_BUILD) $(IDIR_PRIVOXY) $(PACKAGE_DIR)
