include $(TOPDIR)/rules.mk

PKG_NAME:=ucentral-event
PKG_RELEASE:=1

#PKG_SOURCE_URL=https://github.com/blogic/ucentral-event.git
3PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-04-13
PKG_SOURCE_VERSION:=863be47d264ea3fefccadfcfa53c1af53f54dd3b

PKG_MAINTAINER:=John Crispin <john@phrozen.org>
PKG_LICENSE:=BSD-3-Clause

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/ucentral-event
  SECTION:=ucentral
  CATEGORY:=uCentral
  TITLE:=uCentral event gathering daemon
  DEPENDS:=+libubox +libubus +libuci +libblobmsg-json
endef

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include \
	-I$(STAGING_DIR)/usr/include/libnl-tiny

define Package/ucentral-event/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ucentral-event $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/ucentral-event $(1)/etc/init.d/
	$(INSTALL_DATA) ./files/event $(1)/etc/config/
endef

$(eval $(call BuildPackage,ucentral-event))
