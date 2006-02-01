# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=raddump
PKG_VERSION:=0.3.1
PKG_RELEASE:=1
PKG_MD5SUM:=f8c29c67141ea78bb6ae8b97d5149480
PKG_SOURCE_URL:=@SF/raddump
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,RADDUMP,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
                $(TARGET_CONFIGURE_OPTS) \
                CFLAGS="$(TARGET_CFLAGS)" \
                ./configure \
                ac_cv_c_bigendian=no \
                ac_cv_func_malloc_0_nonnull=yes \
                ac_cv_func_realloc_0_nonnull=yes \
                $(DISABLE_NLS) \
                --target=$(GNU_TARGET_NAME) \
                --host=$(GNU_TARGET_NAME) \
                --build=$(GNU_HOST_NAME) \
                --prefix=/usr \
                --exec-prefix=/usr \
                --bindir=/usr/bin \
                --sbindir=/usr/sbin \
                --libexecdir=/usr/lib \
                --sysconfdir=/etc \
                --datadir=/usr/share \
                --localstatedir=/var \
		--mandir=/usr/man \
                --infodir=/usr/info \
                --program-prefix="" \
		--with-gnu-ld \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) DESTDIR=$(PKG_INSTALL_DIR) -C $(PKG_BUILD_DIR) install
	touch $@

$(IPKG_RADDUMP):
	mkdir -p $(IDIR_RADDUMP)
	$(CP) $(PKG_INSTALL_DIR)/* $(IDIR_RADDUMP)/
	$(RSTRIP) $(IDIR_RADDUMP)
	$(IPKG_BUILD) $(IDIR_RADDUMP) $(PACKAGE_DIR)
