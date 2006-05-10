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

include $(TOPDIR)/package/rules.mk

define Package/pcmcia-cs
SECTION:=base
# CATEGORY:=FIXME
DEFAULT:=n
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

define Package/pcmcia-cs/install
	install -m0755 -d $(IDIR_PCMCIA_UTILS)/etc
	install -m0755 -d $(IDIR_PCMCIA_UTILS)/usr/sbin
	install -m0644 $(PKG_INSTALL_DIR)/etc/pcmcia $(IDIR_PCMCIA_UTILS)/etc/
	install -m0755 $(PKG_INSTALL_DIR)/sbin/cardctl $(IDIR_PCMCIA_UTILS)/usr/sbin/
	install -m0755 $(PKG_INSTALL_DIR)/sbin/cardmgr $(IDIR_PCMCIA_UTILS)/usr/sbin/
endef

$(eval $(call BuildPackage,pcmcia-cs))
