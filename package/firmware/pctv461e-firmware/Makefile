include $(TOPDIR)/rules.mk

PKG_NAME:=pctv461e-firmware
PKG_SOURCE_DATE:=2013-11-06
PKG_VERSION:=3.B

PKG_SOURCE:=dvb-demod-m88ds3103.fw
PKG_SOURCE_URL:=http://palosaari.fi/linux/v4l-dvb/firmware/M88DS3103/3.B/
PKG_HASH:=4767ab80ceba4a66315cbec2a07ae1f7ebbd19c5758fd098b932e02c9108eff9

include $(INCLUDE_DIR)/package.mk

define Package/pctv461e-firmware
  CATEGORY:=Firmware
  SECTION:=firmware
  TITLE:=PCTV 461e DVB-S2 demodulator firmware
  URL:=$(PKG_SOURCE_URL)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile
endef

define Package/pctv461e-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_SOURCE) $(1)/lib/firmware
endef

$(eval $(call BuildPackage,pctv461e-firmware))
