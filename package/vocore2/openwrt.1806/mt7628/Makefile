# All rights reserved.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mt7628
P4REV:=120395
PKG_VERSION:=p4rev-$(P4REV)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://github.com/saviourxx/openwrt_dl/raw/master/ \
		https://github.com/guoxiaoqiao/mtk-openwrt-sdk/raw/master/dl/ \
		http://vonger.cn/misc/vocore2/
PKG_MD5SUM:=873ac1eccb0a9f5ca1c8c1b5e9ff5197
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_KCONFIG:= \
	MT_WIFI MT_WIFI_PATH FIRST_IF_EEPROM_PROM FIRST_IF_EEPROM_EFUSE \
	FIRST_IF_EEPROM_FLASH RT_FIRST_CARD_EEPROM RT_SECOND_CARD_EEPROM \
	MULTI_INF_SUPPORT WIFI_BASIC_FUNC WSC_INCLUDED WSC_V2_SUPPORT \
	DOT11N_DRAFT3 DOT11W_PMF_SUPPORT LLTD_SUPPORT QOS_DLS_SUPPORT \
	WAPI_SUPPORT IGMP_SNOOP_SUPPORT BLOCK_NET_IF RATE_ADAPTION \
	NEW_RATE_ADAPT_SUPPORT AGS_SUPPORT IDS_SUPPORT WIFI_WORKQUEUE \
	WIFI_SKB_RECYCLE LED_CONTROL_SUPPORT ATE_SUPPORT MEMORY_OPTIMIZATION \
	UAPSD RLT_MAC RLT_BBP RLT_RF RTMP_MAC RTMP_BBP RTMP_RF RTMP_PCI_SUPPORT \
	RTMP_USB_SUPPORT RTMP_RBUS_SUPPORT WIFI_MODE_AP WIFI_MODE_STA \
	WIFI_MODE_BOTH 	MT_AP_SUPPORT WDS_SUPPORT MBSS_SUPPORT \
	NEW_MBSSID_MODE ENHANCE_NEW_MBSSID_MODE APCLI_SUPPORT \
	MAC_REPEATER_SUPPORT CON_WPS_SUPPORT LLTD_SUPPORT COC_SUPPORT MT_MAC SNIFFER_SUPPORT 
PKG_CONFIG_DEPENDS:=$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_$c),CONFIG_$(c)))

include $(INCLUDE_DIR)/package.mk

TAR_CMD=$(HOST_TAR) -C $(1)/ $(TAR_OPTIONS)

define KernelPackage/mt7628
  TITLE:=MTK MT7628 wifi driver
  FILES:=$(PKG_BUILD_DIR)/build/mt7628.ko
  SUBMENU:=Wireless Drivers
  MENU:=1
  AUTOLOAD:=$(call AutoLoad,98,mt7628)
endef

define KernelPackage/mt7628/config
	source "$(SOURCE)/config.in"
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" V=1 \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)/build/" \
		$(foreach c, $(PKG_KCONFIG),$(if $(CONFIG_MT7628_$c),CONFIG_$(c)=$(CONFIG_MT7628_$(c))))\
		modules
endef

define KernelPackage/mt7628/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/ant1 $(1)/usr/bin
	$(INSTALL_BIN) ./files/ant2 $(1)/usr/bin
	$(INSTALL_DIR) $(1)/lib/netifd/wireless
	$(INSTALL_BIN) ./files/ralink.sh $(1)/lib/netifd/wireless/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) ./files/wireless $(1)/etc/config/
	$(INSTALL_DIR) $(1)/etc/wireless/mt7628/
	$(INSTALL_BIN) ./files/mt7628.dat $(1)/etc/wireless/mt7628/
	echo p$(P4REV) > $(1)/etc/wireless/mt7628/version
endef

$(eval $(call KernelPackage,mt7628))
