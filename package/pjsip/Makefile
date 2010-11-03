
include $(TOPDIR)/rules.mk

PKG_NAME:=pjsip
PKG_VERSION:=1.6
PKG_RELEASE:=1

PKG_SOURCE:=pjproject-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.pjsip.org/release/1.6/
#PKG_MD5SUM:=9b7dc52656f5cbec846a7ba3299f73bd
PKG_INSTALL:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/pjproject-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/pjsip
  SECTION:=lib
  CATEGORY:=Libraries
  TITLE:=Voip lib
  URL:=http://www.pjsip.org/
  DEPENDS:=+libuuid
endef

CONFIGURE_ARGS += \
	--disable-sound \
	--disable-oss \
	--disable-ext-sound \
	--disable-speex-aec \
	--enable-g711-codec \
	--disable-l16-codec \
	--disable-gsm-codec \
	--disable-g722-codec \
	--disable-g7221-codec \
	--disable-speex-codec \
	--disable-ilbc-coder \
	--disable-libsamplerate \
	--disable-ipp \
	--disable-ssl \

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{include,lib}
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* \
		$(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* \
		$(1)/usr/lib/
endef

define Package/pjsip/install
	$(INSTALL_DIR) $(1)/usr/sbin
endef

define Build/Compile
	$(MAKE_VARS) $(MAKE) -C $(PKG_BUILD_DIR)/$(MAKE_PATH)
endef


$(eval $(call BuildPackage,pjsip))
