include $(TOPDIR)/rules.mk

PKG_NAME:=speedtestpp
PKG_VERSION:=1.14
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/taganaka/SpeedTest.git
PKG_SOURCE_DATE:=2021-12-06
PKG_SOURCE_VERSION:=0f63cfbf7ce8d64ea803bf143b957eae76323405
PKG_MIRROR_HASH:=4221584dc3e1e31f2560ef347298a2bcca3ac2331049970b7bd7d742e4e1825f

PKG_MAINTAINER:=Oskari Rauta <oskari.rauta@gmail.com>
PKG_LICENSE:=MIT

CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_OPTIONS += -DCMAKE_BUILD_TYPE=Release .

define Package/speedtestpp
  SECTION:=net
  CATEGORY:=Network
  TITLE:=speedtest++
  DEPENDS:=+libcurl +libxml2 +libopenssl +libstdcpp $(ICONV_DEPENDS)
  CONFLICTS:=python3-speedtest-cli
  URL:=https://github.com/taganaka/SpeedTest
endef

define Package/speedtestpp/description
  Yet another unofficial speedtest.net client cli interface
endef

define Package/speedtestpp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/SpeedTest $(1)/usr/bin/speedtest
endef

$(eval $(call BuildPackage,speedtestpp))
