# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=dosfstools
PKG_VERSION:=2.11
PKG_RELEASE:=1
PKG_MD5SUM:=407d405ade410f7597d364ab5dc8c9f6

PKG_SOURCE_URL:=ftp://ftp.uni-erlangen.de/pub/Linux/LOCAL/dosfstools/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).src.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,DOSFSTOOLS,dosfstools,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		OPTFLAGS="$(TARGET_CFLAGS) -D_FILE_OFFSET_BITS=64" \
		PREFIX="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_DOSFSTOOLS):
	install -d -m0755 $(IDIR_DOSFSTOOLS)/sbin
	$(CP) $(PKG_INSTALL_DIR)/sbin/dosfsck $(IDIR_DOSFSTOOLS)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/fsck.{msdos,vfat} $(IDIR_DOSFSTOOLS)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/mkdosfs $(IDIR_DOSFSTOOLS)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/mkfs.{msdos,vfat} $(IDIR_DOSFSTOOLS)/sbin/
	$(RSTRIP) $(IDIR_DOSFSTOOLS)
	$(IPKG_BUILD) $(IDIR_DOSFSTOOLS) $(PACKAGE_DIR)

