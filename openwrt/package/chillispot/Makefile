# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=chillispot
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_MD5SUM:=997827a9302a85920cfbc1334092ac0c

PKG_SOURCE_URL:=http://www.chillispot.org/download
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,CHILLISPOT,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		ac_cv_func_malloc_0_nonnull=yes \
		ac_cv_func_memcmp_working=yes \
		ac_cv_func_setvbuf_reversed=no \
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

$(IPKG_CHILLISPOT):
	install -m0755 -d $(IDIR_CHILLISPOT)/etc/init.d
	install -m0755 ./files/$(PKG_NAME).init $(IDIR_CHILLISPOT)/etc/init.d/chilli
	install -m0755 -d $(IDIR_CHILLISPOT)/etc
	install -m0644 $(PKG_BUILD_DIR)/doc/chilli.conf $(IDIR_CHILLISPOT)/etc/
	install -m0755 -d $(IDIR_CHILLISPOT)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/chilli $(IDIR_CHILLISPOT)/usr/sbin/
	$(RSTRIP) $(IDIR_CHILLISPOT)
	$(IPKG_BUILD) $(IDIR_CHILLISPOT) $(PACKAGE_DIR)
