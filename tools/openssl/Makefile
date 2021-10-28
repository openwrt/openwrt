include $(TOPDIR)/rules.mk

PKG_NAME:=openssl
PKG_BASE:=1.1.1
PKG_BUGFIX:=l
PKG_VERSION:=$(PKG_BASE)$(PKG_BUGFIX)
PKG_HASH:=0b7a3e5e59c34827fe0c3a74b7ec8baef302b98fa80088d7f9153aa16fa76bd1
PKG_RELEASE:=1

PKG_CPE_ID:=cpe:/a:openssl:openssl

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:= \
        http://www.openssl.org/source/ \
        http://www.openssl.org/source/old/$(PKG_BASE)/ \
        http://ftp.fi.muni.cz/pub/openssl/source/ \
        http://ftp.fi.muni.cz/pub/openssl/source/old/$(PKG_BASE)/ \
        ftp://ftp.pca.dfn.de/pub/tools/net/openssl/source/ \
        ftp://ftp.pca.dfn.de/pub/tools/net/openssl/source/old/$(PKG_BASE)/

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

# default target is gcc if we don't know the host system
OPENSSL_TARGET:=gcc
ifeq ($(HOST_OS),Darwin)
OPENSSL_TARGET:=darwin-$(HOST_ARCH)-cc
endif
ifeq ($(HOST_OS),Linux)
OPENSSL_TARGET:=linux-$(HOST_ARCH)
endif

HOSTCC := $(HOSTCC_NOCACHE)
HOST_CFLAGS += $(HOST_FPIC)
OPENSSL_OPTIONS := no-shared no-hw no-asm threads

define Host/Configure
	(cd $(HOST_BUILD_DIR); \
		./Configure $(OPENSSL_TARGET) \
		--prefix=$(STAGING_DIR_HOST) \
		--libdir=lib \
		$(HOST_CPPFLAGS) \
		$(HOST_LDFLAGS) \
		$(OPENSSL_OPTIONS) \
	)
endef

define Host/Compile
	+$(MAKE) $(PKG_JOBS) -C $(HOST_BUILD_DIR) \
		CC="$(HOSTCC)" \
		install_dev
endef

define Host/Install
	:
endef

$(eval $(call HostBuild))
