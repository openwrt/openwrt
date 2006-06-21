# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=haserl
PKG_VERSION:=0.8.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/haserl
PKG_MD5SUM:=bd9195d086566f56634c0bcbbbcbebea
PKG_CAT:=zcat

include $(TOPDIR)/include/package.mk

define Package/haserl
SECTION:=base
# CATEGORY:=FIXME
TITLE:=A CGI wrapper to embed shell scripts in HTML documents
DESCRIPTION:=A CGI wrapper to embed shell scripts in HTML documents.
URL:=http://haserl.sourceforge.net/
endef

define Package/haserl/install
	install -m0755 -d $(1)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/src/haserl $(1)/usr/bin/
endef

$(eval $(call BuildPackage,haserl))
