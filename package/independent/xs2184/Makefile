include $(TOPDIR)/rules.mk


PKG_NAME:=xs2184
PKG_VERSION:=1.0.1
PKG_RELEASE:=1

#PKG_SOURCE_PROTO:=git
#PKG_SOURCE_URL:=https://gitlab.com/hotray/iot-gw/xs2184.git
#PKG_SOURCE_VERSION:=012d3dc2fcbb9a46751e5bc476758948aff5522d
#PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)
#PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.xz

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_MAINTAINER:=martin.ken <martin@tt-cool.com>

include $(INCLUDE_DIR)/package.mk

ifeq ($(CONFIG_BIG_ENDIAN),y)
TARGET_CFLAGS+= -DHAVE_BIG_ENDIAN=1
endif
TARGET_CFLAGS+= -D_GNU_SOURCE

define Package/xs2184
  SECTION:=Applications
  CATEGORY:=Independent
  TITLE:=XinSheng PoE/PSE i2c driver
  URL:=http://www.tt-cool.com/xs2184
  DEPENDS:=+i2c-tools @TARGET_ramips_mt7621
endef

define Package/xs2184/description
  ZheJiang XinSheng PoE/PSE SoC devices drivers;
endef

# Nothing just to be sure
# define Build/Configure
# endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./files $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/files \
		$(TARGET_CONFIGURE_OPTS) \
		TARGET=Linux \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/xs2184/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/xs2184 $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/xs2184.init $(1)/etc/init.d/xs2184

endef

$(eval $(call BuildPackage,xs2184))
