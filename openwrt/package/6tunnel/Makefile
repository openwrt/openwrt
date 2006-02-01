# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=6tunnel
PKG_VERSION:=0.11rc1
PKG_RELEASE:=1
PKG_MD5SUM:=b325fa9d238e32195fbb3fc3646b0d28

PKG_SOURCE_URL:=http://toxygen.net/6tunnel/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-0.11
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,6TUNNEL,6tunnel,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		ac_cv_path_STRIP=$(STRIP) \
		ac_cv_func_vsnprintf=yes \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--program-prefix="" \
			--program-suffix="" \
			--prefix=/usr \
			--exec-prefix=/usr \
			--bindir=/usr/sbin \
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
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		default
	touch $@

$(IPKG_6TUNNEL):
	install -d -m0755 $(IDIR_6TUNNEL)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/6tunnel $(IDIR_6TUNNEL)/usr/sbin/
	$(RSTRIP) $(IDIR_6TUNNEL)/
	$(IPKG_BUILD) $(IDIR_6TUNNEL) $(PACKAGE_DIR)

