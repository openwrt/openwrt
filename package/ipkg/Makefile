# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ipkg
PKG_VERSION:=0.99.145
PKG_RELEASE:=1
PKG_MD5SUM:=d98799dbcf911f90a7c3c897713cc37a

PKG_SOURCE_URL:=http://www.handhelds.org/packages/ipkg
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,IPKG,ipkg,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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
		  --bindir=/bin \
		  --datadir=/usr/share \
		  --includedir=/usr/include \
		  --infodir=/usr/share/info \
		  --libdir=/lib \
		  --libexecdir=/lib \
		  --localstatedir=/var \
		  --mandir=/usr/share/man \
		  --sbindir=/usr/sbin \
		  --sysconfdir=/etc \
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
	)
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	touch $(PKG_BUILD_DIR)/.built

$(IPKG_IPKG):
	install -d -m0755 $(IDIR_IPKG)/bin
	cp -fpR $(PKG_INSTALL_DIR)/bin/ipkg-cl $(IDIR_IPKG)/bin/ipkg
	install -d -m0755 $(IDIR_IPKG)/lib
	cp -fpR $(PKG_INSTALL_DIR)/lib/libipkg.so.* $(IDIR_IPKG)/lib/
	$(RSTRIP) $(IDIR_IPKG)
	$(IPKG_BUILD) $(IDIR_IPKG) $(PACKAGE_DIR)
