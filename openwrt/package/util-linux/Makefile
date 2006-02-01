# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=fdisk
PKG_VERSION:=2.12r
PKG_RELEASE:=1
PKG_MD5SUM:=c261230b27fc0fbcc287c76884caf2d3

PKG_SOURCE_URL:=ftp://ftp.kernel.org/pub/linux/utils/util-linux/ \
	http://ftp.kernel.org/pub/linux/utils/util-linux/ \
	ftp://ftp.de.kernel.org/pub/linux/utils/util-linux/ \
	http://ftp.de.kernel.org/pub/linux/utils/util-linux/
PKG_SOURCE:=util-linux-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/util-linux-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,FDISK,fdisk,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,LOSETUP,losetup,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SWAP_UTILS,swap-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		./configure \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		OPT="$(TARGET_CFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		INSTALLSUID="install -m 4755" \
		all install
	touch $@

$(IPKG_FDISK):
	install -d -m0755 $(IDIR_FDISK)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/sbin/fdisk $(IDIR_FDISK)/usr/sbin/
	$(RSTRIP) $(IDIR_FDISK)
	$(IPKG_BUILD) $(IDIR_FDISK) $(PACKAGE_DIR)

$(IPKG_LOSETUP):
	install -d -m0755 $(IDIR_LOSETUP)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/sbin/losetup $(IDIR_LOSETUP)/usr/sbin/
	$(RSTRIP) $(IDIR_LOSETUP)
	$(IPKG_BUILD) $(IDIR_LOSETUP) $(PACKAGE_DIR)

$(IPKG_SWAP_UTILS):
	install -d -m0755 $(IDIR_SWAP_UTILS)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/sbin/mkswap $(IDIR_SWAP_UTILS)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/swap{on,off} $(IDIR_SWAP_UTILS)/usr/sbin/
	$(RSTRIP) $(IDIR_SWAP_UTILS)
	$(IPKG_BUILD) $(IDIR_SWAP_UTILS) $(PACKAGE_DIR)

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
