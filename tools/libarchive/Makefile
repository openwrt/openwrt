include $(TOPDIR)/rules.mk

PKG_NAME:=libarchive
PKG_VERSION:=3.4.2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://github.com/$(PKG_NAME)/$(PKG_NAME)/releases/download/v$(PKG_VERSION)/ \
		https://www.$(PKG_NAME).org/downloads/
PKG_HASH:=d8e10494b4d3a15ae9d67a130d3ab869200cfd60b2ab533b391b0a0d5500ada1

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS += \
	--disable-shared \
	--enable-static \
	--enable-bsdtar=static \
	--enable-bsdcat=static \
	--disable-bsdcpio \
	--disable-rpath \
	--enable-posix-regex-lib=libc \
	--disable-xattr \
	--disable-acl \
	--without-bz2lib \
	--without-libb2 \
	--without-iconv \
	--without-lz4 \
	--without-zstd \
	--without-cng \
	--without-nettle \
	--without-openssl \
	--without-xml2 \
	--without-expat

define Host/Install
	$(CP) $(HOST_BUILD_DIR)/bsdtar $(STAGING_DIR_HOST)/bin/
	$(CP) $(HOST_BUILD_DIR)/bsdcat $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/bsdtar
	rm -f $(STAGING_DIR_HOST)/bin/bsdcat
endef

$(eval $(call HostBuild))
