include $(TOPDIR)/rules.mk

PKG_NAME:=wccpd
PKG_VERSION:=0.2
PKG_RELEASE:=1
PKG_MD5SUM:=5f15c274de61dfb88e0dbfc1ccbe6b67
PKG_SOURCE_URL:=@SF/wccpd
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,WCCPD,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
                $(TARGET_CONFIGURE_OPTS) \
                CFLAGS="$(TARGET_CFLAGS)" \
		ac_cv_func_setpgrp_void=yes \
                ./configure \
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
				--mandir=/usr/man \
                --infodir=/usr/info \
                --program-prefix="" \
				--with-gnu-ld \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) DESTDIR=$(PKG_INSTALL_DIR) -C $(PKG_BUILD_DIR) install
	touch $@

$(IPKG_WCCPD):
	mkdir -p $(IDIR_WCCPD)
	cp -fpR $(PKG_INSTALL_DIR)/* $(IDIR_WCCPD)/
	$(RSTRIP) $(IDIR_WCCPD)
	$(IPKG_BUILD) $(IDIR_WCCPD) $(PACKAGE_DIR)
