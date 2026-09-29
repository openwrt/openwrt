include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/package.mk

PKG_NAME:=zte-spifc-probe
PKG_RELEASE:=1

define Package/zte-spifc-probe
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Read-only ZTE SPIFC MMIO watcher
endef

define Package/zte-spifc-probe/description
  Small static ARM probe for observing the ZTE SPI flash controller.
  It only reads /dev/mem and never writes controller registers or flash.
endef

define Build/Prepare
	$(CP) ./src/spifc_raw_watch.S $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) -nostdlib -static -Wl,-e,_start \
		$(PKG_BUILD_DIR)/spifc_raw_watch.S \
		-o $(PKG_BUILD_DIR)/spifc-raw-watch
	$(TARGET_STRIP) $(PKG_BUILD_DIR)/spifc-raw-watch
endef

define Package/zte-spifc-probe/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/spifc-raw-watch $(1)/usr/bin/
endef

$(eval $(call BuildPackage,zte-spifc-probe))