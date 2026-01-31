# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=broadcom-sprom
PKG_SOURCE_DATE:=2023-04-27
PKG_SOURCE_VERSION:=d36f7fcb37711106a3a2652b576436cb09aa5b6f
PKG_MIRROR_HASH:=fce49b1deb40e336fa3cceca7d6c1a062e38ca3c206240bd0acb6c62e3f58ca6
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/openwrt/broadcom-sprom.git

PKG_MAINTAINER:=Álvaro Fernández Rojas <noltari@gmail.com>

include $(INCLUDE_DIR)/package.mk

define Package/broadcom-sprom-default
  SECTION:=firmware
  CATEGORY:=Firmware
endef

define Build/Compile
	true
endef

# BCM4306 Fallback SPROM
define Package/broadcom-4306-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM4306 Fallback SPROM
endef

define Package/broadcom-4306-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm4306-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-4306-sprom))

# BCM4313 Fallback SPROM
define Package/broadcom-4313-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM4313 Fallback SPROM
endef

define Package/broadcom-4313-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm4313-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-4313-sprom))

# BCM4318 Fallback SPROM
define Package/broadcom-4318-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM4318 Fallback SPROM
endef

define Package/broadcom-4318-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm4318-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-4318-sprom))

# BCM4321 Fallback SPROM
define Package/broadcom-4321-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM4321 Fallback SPROM
endef

define Package/broadcom-4321-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm4321-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-4321-sprom))

# BCM4322 Fallback SPROM
define Package/broadcom-4322-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM4322 Fallback SPROM
endef

define Package/broadcom-4322-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm4322-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-4322-sprom))

# BCM4331 Fallback SPROM
define Package/broadcom-4331-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM4331 Fallback SPROM
endef

define Package/broadcom-4331-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm4331-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-4331-sprom))

# BCM4360 Fallback SPROM
define Package/broadcom-4360-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM4360 Fallback SPROM
endef

define Package/broadcom-4360-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm4360-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-4360-sprom))

# BCM6362 Fallback SPROM
define Package/broadcom-6362-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM6362 Fallback SPROM
endef

define Package/broadcom-6362-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm6362-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-6362-sprom))

# BCM43112 Fallback SPROM
define Package/broadcom-43112-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM43112 Fallback SPROM
endef

define Package/broadcom-43112-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm43112-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-43112-sprom))

# BCM43131 Fallback SPROM
define Package/broadcom-43131-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM43131 Fallback SPROM
endef

define Package/broadcom-43131-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm43131-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-43131-sprom))

# BCM43217 Fallback SPROM
define Package/broadcom-43217-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM43217 Fallback SPROM
endef

define Package/broadcom-43217-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm43217-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-43217-sprom))

# BCM43222 Fallback SPROM
define Package/broadcom-43222-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM43222 Fallback SPROM
endef

define Package/broadcom-43222-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm43222-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-43222-sprom))

# BCM43224 Fallback SPROM
define Package/broadcom-43224-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM43224 Fallback SPROM
endef

define Package/broadcom-43224-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm43224-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-43224-sprom))

# BCM43225 Fallback SPROM
define Package/broadcom-43225-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM43225 Fallback SPROM
endef

define Package/broadcom-43225-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm43225-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-43225-sprom))

# BCM43226 Fallback SPROM
define Package/broadcom-43226-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM43226 Fallback SPROM
endef

define Package/broadcom-43226-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm43226-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-43226-sprom))

# BCM43227 Fallback SPROM
define Package/broadcom-43227-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM43227 Fallback SPROM
endef

define Package/broadcom-43227-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm43227-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-43227-sprom))

# BCM43228 Fallback SPROM
define Package/broadcom-43228-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM43228 Fallback SPROM
endef

define Package/broadcom-43228-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm43228-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-43228-sprom))

# BCM43428 Fallback SPROM
define Package/broadcom-43428-sprom
  $(Package/broadcom-sprom-default)
  TITLE:=BCM43428 Fallback SPROM
endef

define Package/broadcom-43428-sprom/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bcm43428-sprom.bin \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,broadcom-43428-sprom))
