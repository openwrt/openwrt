# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-diag
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/kmod-diag
SECTION:=drivers
CATEGORY:=Drivers
DEFAULT:=y
DEPENDS:=@LINUX_2_4_BRCM
TITLE:=Driver for router LEDs and Buttons
DESCRIPTION:=Driver for router LEDs and Buttons
VERSION:=$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		PATH="$(TARGET_PATH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="-DBCMDRIVER -I$(LINUX_DIR)/arch/mips/bcm947xx/include" \
		modules
endef
	
define Package/kmod-diag/install
	install -d -m0755 $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/diag.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
endef

$(eval $(call BuildPackage,kmod-diag))
