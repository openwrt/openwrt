include $(TOPDIR)/rules.mk

PKG_NAME:=npth
PKG_VERSION:=1.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://gnupg.org/ftp/gcrypt/$(PKG_NAME)
PKG_HASH:=8bd24b4f23a3065d6e5b26e98aba9ce783ea4fd781069c1b35d149694e90ca3e

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared

define Host/Compile
	$(call Host/Compile/Default)
endef

define Host/Install
	$(call Host/Install/Default)
endef

$(eval $(call HostBuild))

