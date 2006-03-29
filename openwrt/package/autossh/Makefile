# $Id$
#
# Adjust SSH below to suit.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=autossh
PKG_VERSION:=1.3
PKG_RELEASE:=1
PKG_MD5SUM:=0e7ec62eeca5d1b197f3ef2d0f0d417f
PKG_SOURCE_URL:=http://www.harding.motd.ca/autossh
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

VER=1.3
SSH=/usr/bin/ssh

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,AUTOSSH,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	cp $(PKG_BUILD_DIR)/Makefile.linux $(PKG_BUILD_DIR)/Makefile
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) CFLAGS="$(TARGET_CFLAGS) -Wall -D\"SSH_PATH=\\\"$(SSH)\\\"\" -D\"VER=\\\"$(PKG_VERSION)\\\"\"" \
		BINDIR=/usr/bin MANDIR=/usr/man
	touch $@

$(IPKG_AUTOSSH):
	mkdir -p $(IDIR_AUTOSSH)/usr/bin
	cp $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_AUTOSSH)/usr/bin/
	$(RSTRIP) $(IDIR_AUTOSSH)
	$(IPKG_BUILD) $(IDIR_AUTOSSH) $(PACKAGE_DIR)
