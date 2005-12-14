# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=madplay
PKG_VERSION:=0.15.2b
PKG_RELEASE:=1
PKG_MD5SUM:=6814b47ceaa99880c754c5195aa1aac1

PKG_SOURCE_URL:=@SF/mad \
	ftp://ftp.mars.org/pub/mpeg/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MADPLAY,madplay,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		BUILD_CC=$(TARGET_CC) HOSTCC=$(HOSTCC) \
		$(TARGET_CONFIGURE_OPTS) \
 		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		LIBS="-lz" \
		ac_cv_linux_vers=2 \
		td_cv_buggygetaddrinfo="no" \
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
			--with-gnu-ld \
			--disable-rpath \
			--disable-debugging \
			--disable-profiling \
			--disable-experimental \
			--without-libiconv-prefix \
			--without-libintl-prefix \
			--without-alsa \
			--without-esd \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_MADPLAY):
	install -m0755 -d $(IDIR_MADPLAY)/usr/bin
	install -m0755 $(PKG_INSTALL_DIR)/usr/bin/madplay $(IDIR_MADPLAY)/usr/bin/
	$(RSTRIP) $(IDIR_MADPLAY)
	$(IPKG_BUILD) $(IDIR_MADPLAY) $(PACKAGE_DIR)
