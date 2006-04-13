# $Id: buildroot-documentation.html,v 1.7.4.3 2005/09/04 10:44:06 wbx Exp $

include $(TOPDIR)/rules.mk

PKG_NAME:=weechat
PKG_VERSION:=0.1.8
PKG_RELEASE:=1
PKG_MD5SUM:=73d6bc211ac32c597f0d278506480a20

PKG_SOURCE_URL:=http://weechat.flashtux.org/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,WEECHAT,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

PKG_DEPEND:=libncurses

ifeq ($(BR2_COMPILE_WEECHAT_WITH_TLS),y)
ENABLE_TLS:=--with-libgnutls-prefix=$(STAGING_DIR)/usr
PKG_DEPEND += , gnutls
else
ENABLE_TLS:=--disable-gnutls
endif

ifneq ($(BR2_COMPILE_WEECHAT_WITH_LUA),y)
DISABLE_LUA:=--disable-lua
else
PKG_DEPEND+=, liblua
endif


$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS)" \
	CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
	LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
	./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--sysconfdir=/etc \
		--disable-nls \
		$(ENABLE_TLS) \
		--with-debug=0 \
		--disable-perl \
		--disable-python \
		--disable-ruby \
		$(DISABLE_LUA) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
	all install
	touch $@

$(IPKG_WEECHAT):
	install -d -m0755 $(IDIR_WEECHAT)/usr/bin
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/weechat-curses $(IDIR_WEECHAT)/usr/bin
	echo "Depends: $(PKG_DEPEND)" >> $(IDIR_WEECHAT)/CONTROL/control
	$(RSTRIP) $(IDIR_WEECHAT)
	$(IPKG_BUILD) $(IDIR_WEECHAT) $(PACKAGE_DIR)
  
mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built
