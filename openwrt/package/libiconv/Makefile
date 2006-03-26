# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libiconv
PKG_VERSION:=1.9.1
PKG_RELEASE:=1
PKG_MD5SUM:=0c99a05e0c3c153bac1c960f78711155

PKG_SOURCE_URL:=@GNU/libiconv
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBICONV,libiconv,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
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
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC)
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		libdir="$(PKG_INSTALL_DIR)/usr/lib" \
		includedir="$(PKG_INSTALL_DIR)/usr/include" \
		install-lib
	touch $@

$(IPKG_LIBICONV):
	install -d -m0755 $(IDIR_LIBICONV)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libiconv.so* $(IDIR_LIBICONV)/usr/lib/
	chmod 0644 $(IDIR_LIBICONV)/usr/lib/*
	$(RSTRIP) $(IDIR_LIBICONV)
	$(IPKG_BUILD) $(IDIR_LIBICONV) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/iconv/libiconv.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include/iconv
	$(CP) $(PKG_INSTALL_DIR)/usr/include/iconv.h $(STAGING_DIR)/usr/include/iconv
	mkdir -p $(STAGING_DIR)/usr/lib/iconv
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libiconv.* $(STAGING_DIR)/usr/lib/iconv
	touch $@
	
install-dev: $(STAGING_DIR)/usr/lib/iconv/libiconv.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/iconv \
		$(STAGING_DIR)/usr/lib/iconv \

compile-targets: install-dev
clean-targets: uninstall-dev
