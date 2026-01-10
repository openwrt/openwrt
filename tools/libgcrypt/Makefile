include $(TOPDIR)/rules.mk

PKG_NAME:=libgcrypt
PKG_VERSION:=1.11.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://www.gnupg.org/ftp/gcrypt/libgcrypt/
PKG_HASH:=6ba59dd192270e8c1d22ddb41a07d95dcdbc1f0fb02d03c4b54b235814330aac

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
