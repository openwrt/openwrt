include $(TOPDIR)/rules.mk

PKG_NAME:=thrift
PKG_VERSION:=0.13.0
PKG_RELEASE:=1

PKG_SOURCE:=v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/apache/thrift/archive/
PKG_HASH:=5da60088e60984f4f0801deeea628d193c33cec621e78c8a43a5d8c4055f7ad9

PKG_LICENSE:=Apache-2.0
PKG_LICENSE_FILES:=LICENSE
PKG_INSTALL:=1
HOST_BUILD_DEPENDS:=boost/host

include $(INCLUDE_DIR)/package.mk

define Package/thrift
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Thrift
  #DEPENDS:=+libfmt +libwangle +librsocket-cpp +libfolly +libfizz +boost
endef

TARGET_CFLAGS=
TARGET_CPPFLAGS=
TARGET_CXXFLAGS+= -std=c++0x

CONFIGURE_ARGS += \
	--without-csharp --without-java --without-erlang \
	--without-python --without-perl --without-php --without-php_extension \
	--without-ruby --without-haskell --without-go --without-d \
	--without-qt4 --without-lua --with-cpp \
	-disable-tests --disable-tutorial \
	--with-boost=$(STAGING_DIR)/usr/include \
	ac_cv_path_THRIFT=$(STAGING_DIR)/usr/include

define Build/Configure
	cd $(PKG_BUILD_DIR); ./bootstrap.sh
	$(call Build/Configure/Default)
endef


define Package/thrift/description
  Facebook's branch of Apache Thrift, including a new C++ server.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/thrift/ $(1)/include/
endef

define Package/thrift/install
	$(INSTALL_DIR) $(1)/usr/lib
endef

$(eval $(call BuildPackage,thrift))
