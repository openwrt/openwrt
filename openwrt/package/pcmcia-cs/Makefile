# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=pcmcia-cs
PKG_VERSION:=3.2.8
PKG_RELEASE:=1
PKG_MD5SUM:=0d6d65be8896eff081aee996049afaa5

PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PCMCIA_UTILS,pcmcia-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	( cd $(PKG_BUILD_DIR); \
		./Configure \
			--noprompt \
			--kernel="$(LINUX_DIR)" \
			--target="$(PKG_INSTALL_DIR)" \
			--arch="$(ARCH)" \
			--ucc="$(TARGET_CC)" \
			--uflags="$(TARGET_CFLAGS)" \
			--srctree \
			--cardbus \
			--noapm \
			--nopnp \
			--notrust \
			--nox11 \
			--sysv --rcdir="/etc" \
	)
	touch $@

$(PKG_BUILD_DIR)/.built: 
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS)" \
		all install
	touch $@

$(IPKG_PCMCIA_UTILS):
	install -m0755 -d $(IDIR_PCMCIA_UTILS)/etc
	$(CP) $(PKG_INSTALL_DIR)/etc/pcmcia $(IDIR_PCMCIA_UTILS)/etc/
	install -m0755 -d $(IDIR_PCMCIA_UTILS)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/sbin/cardctl $(IDIR_PCMCIA_UTILS)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/cardmgr $(IDIR_PCMCIA_UTILS)/usr/sbin/
	$(RSTRIP) $(IDIR_PCMCIA_UTILS)
	$(IPKG_BUILD) $(IDIR_PCMCIA_UTILS) $(PACKAGE_DIR)

