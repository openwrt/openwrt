include $(TOPDIR)/rules.mk

PKG_NAME:=librpc
PKG_VERSION:=0.9.32-rc2
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/uclibc-rpc.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=0a2179bbc0844928f2a0ec01dba93d9b5d6d41a7
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz

ifndef CONFIG_NATIVE_TOOLCHAIN
 ifndef CONFIG_EXTERNAL_TOOLCHAIN
  ifdef CONFIG_USE_UCLIBC
    BUILD_LIBRPC=1
  endif
 endif
endif

ifdef BUILD_LIBRPC
  CMAKE_INSTALL:=1
endif

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/librpc
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=uClibc RPC library
  MDEPENDS:=@USE_UCLIBC
endef

ifdef BUILD_LIBRPC
  define Package/librpc/install
	$(INSTALL_DIR) $(1)/lib/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/librpc.so $(1)/lib/
  endef
else
  define Build/Prepare
  endef

  define Build/Configure
  endef

  define Build/Compile
  endef

  define Build/Install
  endef

  define Package/librpc/install
	true
  endef
endif

$(eval $(call BuildPackage,librpc))
