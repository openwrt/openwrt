# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=vgp
PKG_VERSION:=0.2.1
PKG_RELEASE:=1
PKG_MD5SUM:=b8c25b5dfcb944f78bbc584be9c230c7
PKG_SOURCE_URL:=http://vgpd.freaknet.org/files/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,VGP,vgp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
			--infodir=/usr/info \
			--program-prefix="" \
		);
		touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) DESTDIR=$(PKG_INSTALL_DIR) -C $(PKG_BUILD_DIR) install
	touch $@

$(IPKG_VGP):
	mkdir -p $(IDIR_VGP)
	$(CP) $(PKG_INSTALL_DIR)/* $(IDIR_VGP)/
	$(RSTRIP) $(IDIR_VGP)
	$(IPKG_BUILD) $(IDIR_VGP) $(PACKAGE_DIR)
