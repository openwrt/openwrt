#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=prism54-firmware
PKG_VERSION:=1.0.4.3
PKG_RELEASE:=1

# Prism54 FullMAC firmware (jbnore.free.fr seems to be rather slow, so we use daemonizer.de)
PKG_SOURCE:=$(PKG_VERSION).arm
PKG_SOURCE_URL:=https://daemonizer.de/prism54/prism54-fw/fw-fullmac/
PKG_MD5SUM:=8bd4310971772a486b9784c77f8a6df9

include $(INCLUDE_DIR)/package.mk

define Package/prism54-firmware
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://daemonizer.de/prism54/prism54-fw
  TITLE:=prism54 firmware
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile

endef

define Package/prism54-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_SOURCE) $(1)/lib/firmware/isl3890
endef

$(eval $(call BuildPackage,prism54-firmware))
