#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pjsip
PKG_VERSION:=1.10
PKG_RELEASE:=3

PKG_SOURCE:=pjproject-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.pjsip.org/release/$(PKG_VERSION)/
PKG_MD5SUM:=e215d0637d3422d49a63c2cde6673951

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/pjproject-$(PKG_VERSION)

PKG_BUILD_DEPENDS:=TARGET_lantiq:kmod-ltq-tapi TARGET_lantiq:kmod-ltq-vmmc

include $(INCLUDE_DIR)/package.mk

define Package/pjsip-template
  SECTION:=lib
  CATEGORY:=Libraries
  URL:=http://www.pjsip.org/
  MAINTAINER:=John Crispin <blogic@openwrt.org>
  TITLE:=pjsip-$(1)
  VARIANT:=$(1)
  DEPENDS:=+libuuid $(2)
endef

CONFIGURE_PREFIX=/usr/pjsip-$(BUILD_VARIANT)

ifeq ($(BUILD_VARIANT),oss)
CONFIGURE_ARGS += \
	--disable-floating-point \
	--enable-g711-codec \
	--disable-l16-codec \
	--disable-g722-codec \
	--disable-g7221-codec \
	--disable-gsm-codec \
	--disable-ilbc-coder \
	--disable-libsamplerate \
	--disable-ipp \
	--disable-ssl \
	--enable-oss \
	--enable-sound
endif

ifeq ($(BUILD_VARIANT),ltq-tapi)
CONFIGURE_ARGS += \
	--disable-floating-point \
	--enable-g711-codec \
	--disable-l16-codec \
	--disable-g722-codec \
	--disable-g7221-codec \
	--disable-ilbc-coder \
	--disable-gsm-codec \
	--disable-libsamplerate \
	--disable-ipp \
	--disable-ssl \
	--enable-sound \
	--enable-ltq-tapi
EXTRA_CFLAGS:=-I$(STAGING_DIR)/usr/include/drv_tapi -I$(STAGING_DIR)/usr/include/drv_vmmc
endif

Package/pjsip-oss=$(call Package/pjsip-template,oss,)
Package/pjsip-ltq-tapi=$(call Package/pjsip-template,ltq-tapi,@TARGET_lantiq +TARGET_lantiq:kmod-ltq-tapi +TARGET_lantiq:kmod-ltq-vmmc)

define Build/Prepare
	$(PKG_UNPACK)
	$(Build/Patch)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); autoconf aconfigure.ac > aconfigure)
	$(call Build/Configure/Default)
endef

define Build/Compile
	CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS) $(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)" \
	CXXFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS) $(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS) $(LIBGCC_S) -lm" \
		$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR)/$(MAKE_PATH)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr
	$(CP) $(PKG_INSTALL_DIR)/usr/pjsip-$(BUILD_VARIANT) $(1)/usr
endef

define Package/pjsip-$(BUILD_VARIANT)/install
	echo "foo"
endef

$(eval $(call BuildPackage,pjsip-oss))
$(eval $(call BuildPackage,pjsip-ltq-tapi))
