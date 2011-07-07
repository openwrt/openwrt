#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=tapidemo
PKG_VERSION:=5.1.0.53
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources
PKG_MD5SUM:=c970becc46b2935fb9e18f795d4e8469

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/ltq-tapidemo
  SUBMENU:=Telephony
  SECTION:=net
  CATEGORY:=Network
  TITLE:=TAPIdemo application for Lantiq boards
  URL:=http://www.lantiq.com/
  DEPENDS:=+kmod-ltq-tapi +kmod-ltq-vmmc
  MAINTAINER:=John Crispin <blogic@openwrt.org>
  MENU:=1
endef

define Package/ltq-tapidemo/description
	Voice Access mini-PBX Demo Application
endef

define Package/ltq-tapidemo/config
	source "$(SOURCE)/Config.in"
endef

CONFIGURE_ARGS += \
	ARCH=$(LINUX_KARCH) \
	--enable-linux-26 \
	--enable-kernelincl="$(LINUX_DIR)/include" \
	--with-drvincl="$(STAGING_DIR)/usr/include" \
	--with-ifxos-incl=$(STAGING_DIR)/usr/include/ifxos \
	--with-ifxos-lib=$(STAGING_DIR)/usr/lib \
	$(call autoconf_bool,CONFIG_VOICE_CPE_TAPI_QOS,qos) \
	$(call autoconf_bool,CONFIG_VOICE_CPE_TAPIDEMO_FAX_T,fax-t38) \
	--enable-trace \
	--enable-fs

ifeq ($(CONFIG_VOICE_CPE_TAPIDEMO_BOARD_EASY50712),y)
  CONFIGURE_ARGS += --enable-boardname=EASY50712
endif
ifeq ($(CONFIG_VOICE_CPE_TAPIDEMO_BOARD_EASY50712_V3),y)
  CONFIGURE_ARGS += --enable-boardname=EASY50712_V3
endif
ifeq ($(CONFIG_VOICE_CPE_TAPIDEMO_BOARD_EASY508xx),y)
  CONFIGURE_ARGS += --enable-boardname=EASY508XX
endif
ifeq ($(CONFIG_VOICE_CPE_TAPIDEMO_BOARD_EASY80910),y)
  CONFIGURE_ARGS += --enable-boardname=EASY508XX
endif
ifneq ($(CONFIG_VOICE_CPE_TAPIDEMO_FW_FILE),)
  CONFIGURE_ARGS += --with-fw-file="$(strip $(subst ",, $(CONFIG_VOICE_CPE_TAPIDEMO_FW_FILE)))"
endif
ifneq ($(CONFIG_VOICE_CPE_TAPIDEMO_BBD_FILE), "")
CONFIGURE_ARGS += --with-bbd-file="$(strip $(subst ",, $(CONFIG_VOICE_CPE_TAPIDEMO_BBD_FILE)))"
endif

define Package/ltq-tapidemo/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/tapidemo $(1)/usr/sbin
	$(INSTALL_BIN) ./files/bringup_tapidemo $(1)/etc/init.d/tapidemo
endef

$(eval $(call BuildPackage,ltq-tapidemo))
