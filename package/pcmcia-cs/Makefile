# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=pcmcia-cs
PKG_VERSION:=3.2.8
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=0d6d65be8896eff081aee996049afaa5
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

define Package/pcmcia-cs
SECTION:=base
DEPENDS:=@PCMCIA_SUPPORT
TITLE:=PCMCIA/Cardbus management utilities
DESCRIPTION:=PCMCIA/Cardbus management utilities
URL:=http://pcmcia-cs.sourceforge.net/
endef

define Build/Configure
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
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS)" \
		all install
endef

define Package/pcmcia-cs/install
	install -m0755 -d $(1)/etc
	$(CP) $(PKG_INSTALL_DIR)/etc/pcmcia $(1)/etc/
	install -m0755 -d $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/sbin/cardctl $(1)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/cardmgr $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,pcmcia-cs))
