# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libnet
PKG_VERSION:=1.0.2a
PKG_RELEASE:=7
PKG_MD5SUM:=ddf53f0f484184390e8c2a1bd0853667

PKG_SOURCE_URL:=http://www.packetfactory.net/libnet/dist/deprecated/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/Libnet-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

ENDIAN:=lil
ifeq ($(ARCH),mips)
ENDIAN:=big
endif

$(eval $(call PKG_template,LIBNET,libnet,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		touch configure.in; \
		touch include.m4; \
		touch aclocal.m4; \
		touch Makefile.in; \
		touch configure; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		ac_libnet_have_pf_packet=yes \
		ac_cv_lbl_unaligned_fail=no \
		ac_cv_libnet_endianess=$(ENDIAN) \
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
		$(DISABLE_LARGEFILE) \
		--enable-shared \
		--enable-static \
		--with-pf_packet=yes \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBNET):
	install -m0755 -d $(IDIR_LIBNET)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnet.so.* $(IDIR_LIBNET)/usr/lib/
	$(RSTRIP) $(IDIR_LIBNET)
	$(IPKG_BUILD) $(IDIR_LIBNET) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libnet.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/libnet-config $(STAGING_DIR)/usr/bin/
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libnet* $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnet.{a,so*} $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libnet.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/bin/libnet-config \
		$(STAGING_DIR)/usr/include/libnet*  \
		$(STAGING_DIR)/usr/lib/libnet.{a,so*} \

compile-targets: install-dev
clean-targets: uninstall-dev
