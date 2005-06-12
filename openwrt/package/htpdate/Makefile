# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=htpdate
PKG_VERSION:=0.8.2
PKG_RELEASE:=1
PKG_MD5SUM:=f1d8c0706a6a60f880ce6a0dccbb99a3

PKG_SOURCE_URL:=http://www.clevervest.com/htp/archive/c/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,HTPDATE,htpdate,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)"
	touch $@

$(IPKG_HTPDATE):
	install -d -m0755 $(IDIR_HTPDATE)/etc/default/
	install -m0644 ./files/htpdate.default $(IDIR_HTPDATE)/etc/default/htpdate
	install -d -m0755 $(IDIR_HTPDATE)/etc/init.d/
	install -m0755 ./files/htpdate.init $(IDIR_HTPDATE)/etc/init.d/htpdate
	ln -sf htpdate $(IDIR_HTPDATE)/etc/init.d/S49htpdate
	install -d -m0755 $(IDIR_HTPDATE)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/htpdate $(IDIR_HTPDATE)/usr/sbin/
	$(RSTRIP) $(IDIR_HTPDATE)
	$(IPKG_BUILD) $(IDIR_HTPDATE) $(PACKAGE_DIR)

