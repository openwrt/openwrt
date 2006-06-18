# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=kmod-wlcompat
PKG_RELEASE:=3

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

WLCOMPAT_FLAGS:=$(TARGET_CFLAGS) -D__KERNEL__ -fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0 \
		-mno-abicalls -fno-pic -finline-limit=100000 -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 \
		-Wa,-mips32 -Wa,--trap -DMODULE -mlong-calls -fno-common -I. -I linux-2.4 -funsigned-char -nostdinc \
		-iwithprefix include -I$(LINUX_DIR)/include -I$(LINUX_DIR)/include/asm/gcc -I$(LINUX_DIR)/arch/mips/bcm947xx/include -I$(STAGING_DIR)/usr/include -I./src/include

define Package/kmod-wlcompat
SECTION:=drivers
CATEGORY:=Drivers
DEPENDS:=
TITLE:=Wrapper providing Wireless Extensions for Broadcom wl module
DESCRIPTION:= \\\
A wrapper module, that provides Wireless Extension support for the \\\
proprietary Broadcom wl module.
VERSION:=$(LINUX_VERSION)+$(BOARD)-$(PKG_RELEASE)
endef

define Package/kmod-wlcompat-debug
$(call Package/kmod-wlcompat)
TITLE:=Wrapper providing Wireless Extensions for Broadcom wl module (debug)
DESCRIPTION:= \\\
A wrapper module, that provides Wireless Extension support for the \\\
proprietary Broadcom wl module.
endef

define Build/Compile
	$(TARGET_CC) -c -o $(PKG_BUILD_DIR)/wlcompat.o $(WLCOMPAT_FLAGS) wlcompat.c
	$(TARGET_CC) -c -o $(PKG_BUILD_DIR)/wlcompat-debug.o $(WLCOMPAT_FLAGS) -DDEBUG wlcompat.c
endef

define Package/kmod-wlcompat/install
	install -d -m0755 $(1)/etc/modules.d
	echo "wlcompat" > $(1)/etc/modules.d/30-wlcompat
	install -d -m0755 $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/wlcompat.o $(1)/lib/modules/$(LINUX_VERSION)/
endef

define Package/kmod-wlcompat-debug/install
	install -d -m0755 $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/wlcompat-debug.o $(1)/lib/modules/$(LINUX_VERSION)/
endef

$(eval $(call BuildPackage,kmod-wlcompat))
$(eval $(call BuildPackage,kmod-wlcompat-debug))
