include $(TOPDIR)/rules.mk

PKG_NAME:=p4-bmv2
PKG_VERSION:=1.14.0
PKG_RELEASE:=1

PKG_SOURCE_URL=https://github.com/p4lang/behavioral-model.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2020-12-09
PKG_SOURCE_VERSION:=9982947acb075a18697a77e21e27e805e4167c05

PKG_FIXUP:=autoreconf
PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk

define Package/p4-bmv2
  SECTION:=net
  CATEGORY:=Network
  TITLE:=P4 - Behavioural Model
  URL:=http://p4.org/
  DEPENDS:=+libJudy +boost +boost-filesystem +boost-program_options +boost-system +boost-thread +libpcap +libgmp +libstdcpp
endef

TARGET_CPPFLAGS += -I$(STAGING_DIR)/include/ -std=c++11
CONFIGURE_ARGS += --without-thrift --without-nanomsg

define Package/p4-bmv2/install
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/lib
endef

$(eval $(call BuildPackage,p4-bmv2))
