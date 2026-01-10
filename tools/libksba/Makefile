include $(TOPDIR)/rules.mk

PKG_NAME:=libksba
PKG_VERSION:=1.6.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://gnupg.org/ftp/gcrypt/$(PKG_NAME)
PKG_HASH:=cf72510b8ebb4eb6693eef765749d83677a03c79291a311040a5bfd79baab763

HOST_BUILD_DEPENDS:=libgpg-error/host

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--with-libgpg-error-prefix=$(STAGING_DIR_HOST)

define Host/Compile
	$(call Host/Compile/Default)
endef

define Host/Install
	$(call Host/Install/Default)
endef

$(eval $(call HostBuild))
