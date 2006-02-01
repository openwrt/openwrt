# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=macchanger
PKG_VERSION:=1.5.0
PKG_RELEASE:=1
PKG_MD5SUM:=79b7cdaeca3d8ebafa764c4b0dd03ab7
PKG_SOURCE_URL:=@GNU/macchanger
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MACCHANGER,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
                $(TARGET_CONFIGURE_OPTS) \
                CFLAGS="$(TARGET_CFLAGS)" \
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

$(IPKG_MACCHANGER):
	mkdir -p $(IDIR_MACCHANGER)
	$(CP) $(PKG_INSTALL_DIR)/* $(IDIR_MACCHANGER)/
	$(RSTRIP) $(IDIR_MACCHANGER)
	$(IPKG_BUILD) $(IDIR_MACCHANGER) $(PACKAGE_DIR)
