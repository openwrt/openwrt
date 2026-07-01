include $(TOPDIR)/rules.mk

PKG_NAME:=libassuan
PKG_VERSION:=3.0.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://gnupg.org/ftp/gcrypt/$(PKG_NAME)
PKG_HASH:=d2931cdad266e633510f9970e1a2f346055e351bb19f9b78912475b8074c36f6

HOST_BUILD_DEPENDS:=libgpg-error/host

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--disable-doc \
	--with-gpg-error-prefix=$(STAGING_DIR_HOST)

define Host/Compile
	$(call Host/Compile/Default)
endef

define Host/Install
	$(call Host/Install/Default)
endef

$(eval $(call HostBuild))
