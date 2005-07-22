# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=portmap
PKG_VERSION:=5beta
PKG_RELEASE:=2
PKG_MD5SUM:=781e16ed4487c4caa082c6fef09ead4f

# space separated list or special @SF for sourceforge projects
PKG_SOURCE_URL:=ftp://ftp.porcupine.org/pub/security
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)_$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PORTMAP,portmap,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -DHOSTS_ACCESS -DFACILITY=LOG_DAEMON -DIGNORE_SIGCHLD" \
		WRAP_LIB="-L$(STAGING_DIR)/usr/lib -lwrap" \
		all
	touch $@

$(IPKG_PORTMAP):
	install -d -m0755 $(IDIR_PORTMAP)/etc/init.d 
	install -m0755 ./files/portmap.init $(IDIR_PORTMAP)/etc/init.d/portmap
	ln -sf portmap $(IDIR_PORTMAP)/etc/init.d/S59portmap
	install -d -m0755 $(IDIR_PORTMAP)/usr/sbin 
	install -m0755 $(PKG_BUILD_DIR)/portmap $(IDIR_PORTMAP)/usr/sbin/
	$(RSTRIP) $(IDIR_PORTMAP)
	$(IPKG_BUILD) $(IDIR_PORTMAP) $(PACKAGE_DIR)
