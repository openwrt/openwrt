# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=vsftpd
PKG_VERSION:=1.2.2
PKG_RELEASE:=1
PKG_MD5SUM:=5a38e1c69062e7688fbbdae6d1196de2

PKG_SOURCE_URL:=ftp://vsftpd.beasts.org/users/cevans/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,VSFTPD,vsftpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)"
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	touch $(PKG_BUILD_DIR)/.built

$(IPKG_VSFTPD):
	install -d -m0755 $(IDIR_VSFTPD)/etc
	install -m0644 ./files/vsftpd.conf $(IDIR_VSFTPD)/etc/vsftpd.conf
	install -d -m0755 $(IDIR_VSFTPD)/etc/init.d
	install -m0755 ./files/vsftpd.init $(IDIR_VSFTPD)/etc/init.d/vsftpd
	install -d -m0755 $(IDIR_VSFTPD)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/vsftpd $(IDIR_VSFTPD)/usr/sbin/
	$(RSTRIP) $(IDIR_VSFTPD)
	$(IPKG_BUILD) $(IDIR_VSFTPD) $(PACKAGE_DIR)
