# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=l2tpd
PKG_VERSION:=0.70pre
PKG_UPSTREAM_VERSION:=0.70-pre20031121
PKG_RELEASE:=4.1
PKG_MD5SUM:=3f2707b6e16a8cb72e7bf64f574202fa

PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/l/l2tpd
PKG_SOURCE:=$(PKG_NAME)_$(PKG_UPSTREAM_VERSION).orig.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_UPSTREAM_VERSION).orig
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,L2TPD,l2tpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CC=$(TARGET_CC) \
		CFLAGS="$(TARGET_CFLAGS) -I. -DLINUX -DSANITY -DIP_ALLOCATION"
	touch $@

$(IPKG_L2TPD):
	install -d -m0755 $(IDIR_L2TPD)/etc/l2tpd
	install -m0644 ./files/l2tpd.conf $(IDIR_L2TPD)/etc/l2tpd/
	install -d -m0755 $(IDIR_L2TPD)/etc/ppp
	install -m0644 ./files/options.l2tpd $(IDIR_L2TPD)/etc/ppp/
	install -d -m0755 $(IDIR_L2TPD)/etc/init.d
	install -m0755 ./files/l2tpd.init $(IDIR_L2TPD)/etc/init.d/l2tpd
	install -d -m0755 $(IDIR_L2TPD)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_L2TPD)/usr/sbin/
	$(RSTRIP) $(IDIR_L2TPD)
	$(IPKG_BUILD) $(IDIR_L2TPD) $(PACKAGE_DIR)

