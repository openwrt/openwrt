include $(TOPDIR)/rules.mk

PKG_NAME:=traffic-classifier
PKG_VERSION:=0.1.0
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=Anand Kumar
PKG_BUILD_PARALLEL:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/traffic-classifier
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Edge AI network traffic classifier
  DEPENDS:=+libubox +libubus +libblobmsg-json +libjson-c +libpcap
endef

define Package/traffic-classifier/description
  Real-time network traffic classification daemon for OpenWRT.
  Captures flows on the bridge interface, extracts statistical features
  (packet sizes, inter-arrival times, byte ratios), and classifies
  traffic into application categories (video, gaming, social, browsing).
  Exposes per-client per-SSID results via ubus.
endef

define Package/traffic-classifier/conffiles
/etc/config/traffic-classifier
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) \
		-o $(PKG_BUILD_DIR)/traffic-classifier \
		$(PKG_BUILD_DIR)/main.c \
		$(PKG_BUILD_DIR)/capture.c \
		$(PKG_BUILD_DIR)/flow_table.c \
		$(PKG_BUILD_DIR)/features.c \
		$(PKG_BUILD_DIR)/classifier.c \
		$(PKG_BUILD_DIR)/ubus_api.c \
		$(PKG_BUILD_DIR)/sta_tracker.c \
		-lubox -lubus -lblobmsg_json -ljson-c -lpcap -lm
endef

define Package/traffic-classifier/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/traffic-classifier $(1)/usr/sbin/

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/traffic-classifier.init $(1)/etc/init.d/traffic-classifier

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/traffic-classifier.conf $(1)/etc/config/traffic-classifier

	$(INSTALL_DIR) $(1)/etc/traffic-classifier
	$(INSTALL_DATA) ./files/model_stub.json $(1)/etc/traffic-classifier/model.json
endef

$(eval $(call BuildPackage,traffic-classifier))
