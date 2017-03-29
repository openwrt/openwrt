PKG_VERSION:=2.02~beta2
PKG_RELEASE:=1

PKG_SOURCE:=grub-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://alpha.gnu.org/gnu/grub \
	http://gnualpha.uib.no/grub/ \
	http://mirrors.fe.up.pt/pub/gnu-alpha/grub/ \
	http://www.nic.funet.fi/pub/gnu/alpha/gnu/grub/
PKG_HASH:=f6c702b2a8ea58f27a2b02928bb77973cb5a827af08f63db38c471c0a01b418d

HOST_BUILD_PARALLEL:=1

PKG_SSP:=0

PATCH_DIR := ../patches
HOST_PATCH_DIR := ../patches

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

define Package/grub2/Default
  SUBMENU:=Boot Loaders
  CATEGORY:=Utilities
  SECTION:=utils
  TITLE:=GRand Unified Bootloader
  URL:=http://www.gnu.org/software/grub/
  DEPENDS:=@TARGET_x86||TARGET_x86_64
endef

HOST_BUILD_PREFIX := $(STAGING_DIR_HOST)

CONFIGURE_ARGS += \
	--target=$(REAL_GNU_TARGET_NAME) \
	--disable-werror \
	--disable-nls \
	--disable-device-mapper \
	--disable-libzfs \
	--disable-grub-mkfont

HOST_CONFIGURE_ARGS += \
	--target=$(REAL_GNU_TARGET_NAME) \
	--sbindir="$(STAGING_DIR_HOST)/bin" \
	--disable-werror \
	--disable-libzfs \
	--disable-nls

HOST_MAKE_FLAGS += \
	TARGET_RANLIB=$(TARGET_RANLIB) \
	LIBLZMA=$(STAGING_DIR_HOST)/lib/liblzma.a

define Host/Configure
	$(SED) 's,(RANLIB),(TARGET_RANLIB),' $(HOST_BUILD_DIR)/grub-core/Makefile.in
	$(Host/Configure/Default)
endef

