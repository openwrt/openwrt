# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=mini-sendmail
PKG_VERSION:=1.3.5
PKG_RELEASE:=1
PKG_MD5SUM:=fff344184e98cff0ea4d817da9d29383

PKG_SOURCE_URL:=http://www.acme.com/software/mini_sendmail/
PKG_SOURCE:=mini_sendmail-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/mini_sendmail-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MINI_SENDMAIL,mini-sendmail,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))


$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		OFLAGS="$(TARGET_CFLAGS)" \
		all
	touch $@

$(IPKG_MINI_SENDMAIL):
	install -d -m0755 $(IDIR_MINI_SENDMAIL)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/mini_sendmail $(IDIR_MINI_SENDMAIL)/usr/sbin/
	ln -sf mini_sendmail $(IDIR_MINI_SENDMAIL)/usr/sbin/sendmail
	$(RSTRIP) $(IDIR_MINI_SENDMAIL)
	$(IPKG_BUILD) $(IDIR_MINI_SENDMAIL) $(PACKAGE_DIR)
