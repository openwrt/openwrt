include $(TOPDIR)/rules.mk

PKG_NAME:=autoconf-archive
PKG_VERSION:=2023.02.20

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@GNU/autoconf-archive
PKG_HASH:=71d4048479ae28f1f5794619c3d72df9c01df49b1c628ef85fde37596dc31a33

PKG_GPG_KEYS_DIR:=$(CURDIR)/keys
PKG_SOURCE_SIG:=$(PKG_SOURCE).sig
PKG_VALIDPGPKEYS:=1A4F63A13A4649B632F65EE141BC28FE99089D72

include $(INCLUDE_DIR)/host-build.mk

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
