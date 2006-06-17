# $Id$

include $(TOPDIR)/rules.mk
include $(TOPDIR)/package/kernel.mk

PKG_NAME := kmod-diag
PKG_RELEASE := 1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

DIAG_FLAGS:=$(TARGET_CFLAGS) -D__KERNEL__ -fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0 \
		-mno-abicalls -fno-pic -finline-limit=100000 -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 \
		-Wa,-mips32 -Wa,--trap -Wstrict-prototypes -Wno-trigraphs -DMODULE -mlong-calls -fno-common -I. -I linux-2.4 -nostdinc \
		-iwithprefix include -I$(LINUX_DIR)/include -I$(LINUX_DIR)/include/asm/gcc -c -I$(LINUX_DIR)/arch/mips/bcm947xx/include -DBCMDRIVER

define Package/kmod-diag
 SECTION:=drivers
 CATEGORY:=Drivers
 DEFAULT:=y
 DEPENDS:=@LINUX_2_4_BRCM
 TITLE:=Driver for router LEDs and Buttons
 DESCRIPTION:=Driver for router LEDs and Buttons
 URL:=
 VERSION:=$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Build/Compile
	$(TARGET_CC) $(DIAG_FLAGS) -o $(PKG_BUILD_DIR)/diag.o diag_led.c
endef
	
define Package/kmod-diag/install
	install -d -m0755 $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/diag.o $(1)/lib/modules/$(LINUX_VERSION)/
endef

$(eval $(call BuildPackage,kmod-diag))
