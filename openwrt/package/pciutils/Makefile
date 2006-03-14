# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=pciutils
PKG_VERSION:=2.2.1
PKG_RELEASE:=1
PKG_MD5SUM:=c18e2a5f04e9abae5a42439de294f086

PKG_SOURCE_URL:=http://www.kernel.org/pub/software/utils/pciutils/ \
	ftp://atrey.karlin.mff.cuni.cz/pub/linux/pci
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PCIUTILS,pciutils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) CFLAGS="$(TARGET_CFLAGS)" IDSDIR="/usr/share" HOST=$(GNU_TARGET_NAME)
	touch $@

$(IPKG_PCIUTILS):
	install -d -m0755 $(IDIR_PCIUTILS)/usr/sbin
	install -d -m0755 $(IDIR_PCIUTILS)/usr/share
	$(CP) $(PKG_BUILD_DIR)/{lspci,setpci} $(IDIR_PCIUTILS)/usr/sbin/
	$(CP) $(PKG_BUILD_DIR)/pci.ids $(IDIR_PCIUTILS)/usr/share/
	$(RSTRIP) $(IDIR_PCIUTILS)/
	$(IPKG_BUILD) $(IDIR_PCIUTILS) $(PACKAGE_DIR)
