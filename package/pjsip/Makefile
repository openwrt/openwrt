#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pjsip
PKG_VERSION:=1.8.5
PKG_RELEASE:=2

PKG_SOURCE:=pjproject-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.pjsip.org/release/$(PKG_VERSION)/
PKG_MD5SUM:=f9aa9e58b4d697245eb4bfa7d81a54a3
PKG_INSTALL:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/pjproject-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/pjsip
  SECTION:=lib
  CATEGORY:=Libraries
  TITLE:=Voip lib
  URL:=http://www.pjsip.org/
  DEPENDS:=+libuuid
  MAINTAINER:=John Crispin <blogic@openwrt.org>
endef

define Package/pjsip/config
	source "$(SOURCE)/Config.in"
endef

CONFIGURE_ARGS += \
	--enable-g711-codec \
	--disable-l16-codec \
	--disable-g722-codec \
	--disable-g7221-codec \
	--disable-ilbc-coder \
	--disable-libsamplerate \
	--disable-ipp \
	--disable-ssl \
	$(call autoconf_bool,CONFIG_PJSIP_GSM,gsm-codec) \
	$(call autoconf_bool,CONFIG_PJSIP_SPEEX,speex-aec) \
	$(call autoconf_bool,CONFIG_PJSIP_OSS,oss) \
	$(call autoconf_bool,CONFIG_PJSIP_SOUND,sound) \
	$(call autoconf_bool,CONFIG_PJSIP_DEV_TAPI,ltq-tapi)

ifeq ($(CONFIG_PJSIP_DEV_TAPI),y)
EXTRA_CFLAGS:=-I$(STAGING_DIR)/usr/include/drv_tapi -I$(STAGING_DIR)/usr/include/drv_vmmc
endif

define Build/Configure
	(cd $(PKG_BUILD_DIR); autoconf aconfigure.ac > aconfigure)
	$(call Build/Configure/Default)
endef

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
	CFLAGS="$(EXTRA_CFLAGS) $(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS)" \
		$(MAKE) -C $(PKG_BUILD_DIR)/$(MAKE_PATH)
endef

$(eval $(call BuildPackage,pjsip))
