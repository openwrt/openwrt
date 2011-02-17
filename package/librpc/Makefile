include $(TOPDIR)/rules.mk

PKG_NAME:=librpc
PKG_VERSION:=0.9.32-rc2
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/uclibc-rpc.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=49f63f433ddd1fb7ad1e450094b82aacaa3b0a7d
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
ifdef CONFIG_USE_UCLIBC
  CMAKE_INSTALL:=1
endif

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/librpc
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=uClibc RPC library
  DEPENDS:=@USE_UCLIBC
endef

ifdef CONFIG_USE_UCLIBC
  define Package/librpc/install
	$(INSTALL_DIR) $(1)/lib/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/librpc.so $(1)/lib/
  endef
endif

$(eval $(call BuildPackage,librpc))
