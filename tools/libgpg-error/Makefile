include $(TOPDIR)/rules.mk

PKG_NAME:=libgpg-error
PKG_VERSION:=1.58
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://mirrors.dotsrc.org/gcrypt/libgpg-error \
		http://ring.ksc.gr.jp/archives/net/gnupg/libgpg-error \
		https://www.gnupg.org/ftp/gcrypt/libgpg-error
PKG_HASH:=f943aea9a830a8bd938e5124b579efaece24a3225ff4c3d27611a80ce1260c27

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--disable-doc \
	--disable-languages \
	--disable-tests

define Host/Compile
	$(call Host/Compile/Default)
endef

define Host/Install
	$(call Host/Install/Default)
endef

$(eval $(call HostBuild))
