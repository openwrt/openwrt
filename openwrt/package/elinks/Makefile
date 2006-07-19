# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=elinks
PKG_VERSION:=0.11.1
PKG_RELEASE:=2
PKG_MD5SUM:=c09d726075e4b5810fa07f8654088755

PKG_SOURCE_URL:=http://elinks.or.cz/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,ELINKS,elinks,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
                x=`echo ac_cv_file_./features.conf | tr [:blank:]-/. _` && export $${x}=yes && \
                x=`echo ac_cv_file_$(PKG_BUILD_DIR)/features.conf | tr [:blank:]-/. _` && export $${x}=yes && \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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
			--enable-small\
			--enable-256-colors\
			--disable-backtrace\
			--disable-data\
			--disable-formhist\
			--disable-leds \
			--disable-mailcap\
			--disable-mouse \
			--disable-smb \
			--disable-uri-rewrite\
			--disable-xbel \
			--without-bzlib \
			--without-gpm \
			--without-idn \
			--without-gnutls \
			--without-libiconv \
			--without-lua \
			--with-openssl="$(STAGING_DIR)/usr" \
			--without-spidermonkey\
			--without-x\
			--without-zlib \
			--disable-ipv6 \
	);
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR)\
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld
	touch $@

$(IPKG_ELINKS): 
	mkdir -p $(IDIR_ELINKS)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/src/elinks $(IDIR_ELINKS)/usr/bin/
	$(RSTRIP) $(IDIR_ELINKS)
	$(IPKG_BUILD) $(IDIR_ELINKS) $(PACKAGE_DIR)
