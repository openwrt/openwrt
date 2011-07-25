#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/target.mk

PKG_NAME:=uClibc
PKG_VERSION:=$(call qstrip,$(CONFIG_UCLIBC_VERSION))
ifeq ($(PKG_VERSION),0.9.32)
  PKG_SOURCE_URL:=git://git.busybox.net/uClibc
  PKG_SOURCE_PROTO:=git
  PKG_SOURCE_VERSION:=9152c4d67c763fde5712e2d181d92c0d7e1e2ab9
  PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
  PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
  PKG_MIRROR_MD5SUM:=723dfa4f1f766cbd675d2c9a8238abb0
  LIBC_SO_VERSION:=$(PKG_VERSION).1-git
else
  PKG_SOURCE_URL:=http://www.uclibc.org/downloads
  PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
  LIBC_SO_VERSION:=$(PKG_VERSION)
endif
PATCH_DIR:=./patches-$(PKG_VERSION)
CONFIG_DIR:=./config-$(PKG_VERSION)

PKG_MD5SUM=$(PKG_MD5SUM_$(PKG_VERSION))

HOST_BUILD_DIR:=$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/toolchain-build.mk

HOST_STAMP_BUILT:=$(TOOLCHAIN_DIR)/stamp/.uclibc_installed
HOST_STAMP_INSTALLED:=$(TOOLCHAIN_DIR)/stamp/.uclibc-utils_installed

UCLIBC_TARGET_ARCH:=$(shell echo $(ARCH) | sed -e s'/-.*//' \
		-e 's/i.86/i386/' \
		-e 's/sparc.*/sparc/' \
		-e 's/arm.*/arm/g' \
		-e 's/avr32.*/avr32/g' \
		-e 's/m68k.*/m68k/' \
		-e 's/ppc/powerpc/g' \
		-e 's/v850.*/v850/g' \
		-e 's/sh64/sh/' \
		-e 's/sh[234].*/sh/' \
		-e 's/mips.*/mips/' \
		-e 's/mipsel.*/mips/' \
		-e 's/cris.*/cris/' \
)


define Host/SetToolchainInfo
	$(SED) 's,^\(LIBC_TYPE\)=.*,\1=$(PKG_NAME),' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_URL\)=.*,\1=http://www.uclibc.org/,' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_VERSION\)=.*,\1=$(PKG_VERSION),' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_SO_VERSION\)=.*,\1=$(LIBC_SO_VERSION),' $(TOOLCHAIN_DIR)/info.mk
endef

GEN_CONFIG=$(SCRIPT_DIR)/kconfig.pl -n \
	$(if $(wildcard $(CONFIG_DIR)/common),'+' $(CONFIG_DIR)/common) \
	$(if $(CONFIG_UCLIBC_ENABLE_DEBUG),$(if $(wildcard $(CONFIG_DIR)/debug),'+' $(CONFIG_DIR)/debug)) \
	$(CONFIG_DIR)/$(ARCH)$(strip \
		$(if $(wildcard $(CONFIG_DIR)/$(ARCH).$(BOARD)),.$(BOARD), \
			$(if $(CONFIG_HAS_SPE_FPU),$(if $(wildcard $(CONFIG_DIR)/$(ARCH).e500),.e500))))

define Host/Prepare
	$(call Host/Prepare/Default)
	$(if $(strip $(QUILT)), \
		cd $(HOST_BUILD_DIR); \
		if $(QUILT_CMD) next >/dev/null 2>&1; then \
			$(QUILT_CMD) push -a; \
		fi
	)
	ln -snf $(PKG_NAME)-$(PKG_VERSION) $(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
	$(GEN_CONFIG) > $(HOST_BUILD_DIR)/.config
	$(SED) 's,^KERNEL_HEADERS=.*,KERNEL_HEADERS=\"$(BUILD_DIR_TOOLCHAIN)/linux-dev/include\",g' \
		-e 's,^.*UCLIBC_HAS_FPU.*,UCLIBC_HAS_FPU=$(if $(CONFIG_SOFT_FLOAT),n,y),g' \
		-e 's,^.*UCLIBC_HAS_SOFT_FLOAT.*,UCLIBC_HAS_SOFT_FLOAT=$(if $(CONFIG_SOFT_FLOAT),y,n),g' \
		-e 's,^.*UCLIBC_HAS_SHADOW.*,UCLIBC_HAS_SHADOW=$(if $(CONFIG_SHADOW_PASSWORDS),y,n),g' \
		$(HOST_BUILD_DIR)/.config
	$(MAKE) -C $(HOST_BUILD_DIR) oldconfig KBUILD_HAVE_NLS= HOSTCFLAGS="-DKBUILD_NO_NLS"
	$(MAKE) -C $(HOST_BUILD_DIR)/extra/config conf KBUILD_HAVE_NLS= HOSTCFLAGS="-DKBUILD_NO_NLS"
	PATH='$(TARGET_PATH)' $(MAKE) -C $(HOST_BUILD_DIR) \
		PREFIX="$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/" \
		DEVEL_PREFIX=/ \
		RUNTIME_PREFIX="$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/" \
		HOSTCC="$(HOSTCC)" \
		CC="$(TARGET_CC)" \
		CPU_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(CONFIG_ARCH)" \
		pregen \
		install_headers
endef

define Host/Configure
endef

CPU_CFLAGS = \
	-funsigned-char -fno-builtin -fno-asm \
	--std=gnu99 -ffunction-sections -fdata-sections \
	$(TARGET_CFLAGS)

UCLIBC_MAKE = PATH='$(TARGET_PATH)' $(MAKE) -C $(HOST_BUILD_DIR) \
	$(TARGET_CONFIGURE_OPTS) \
	DEVEL_PREFIX=/ \
	RUNTIME_PREFIX=/ \
	HOSTCC="$(HOSTCC)" \
	CPU_CFLAGS="$(CPU_CFLAGS)" \
	ARCH="$(CONFIG_ARCH)" \
	LIBGCC="$(subst libgcc.a,libgcc_initial.a,$(shell $(TARGET_CC) -print-libgcc-file-name))" \
	DOSTRIP=""

define Host/Compile
	$(SED) 's,^CROSS=.*,CROSS=$(TARGET_CROSS),g' $(HOST_BUILD_DIR)/Rules.mak
	$(UCLIBC_MAKE) $(TOOLCHAIN_JOBS) PREFIX= all
	$(UCLIBC_MAKE) PREFIX="$(TOOLCHAIN_DIR)/" install_runtime install_dev
	$(CP) $(HOST_BUILD_DIR)/libc/libc_so.a $(TOOLCHAIN_DIR)/lib/
	$(CP) $(HOST_BUILD_DIR)/libpthread/*/libpthread_so.a $(TOOLCHAIN_DIR)/lib/
	( cd $(TOOLCHAIN_DIR) ; \
		for d in lib usr/lib ; do \
		  for f in libc.so libpthread.so libgcc_s.so ; do \
		    if [ -f $$$$d/$$$$f -a ! -L $$$$d/$$$$f ] ; then \
		      $(SED) 's,/usr/lib/,,g;s,/lib/,,g' $$$$d/$$$$f ; \
		    fi \
		  done \
		done \
	)
	rm -f \
		$(TOOLCHAIN_DIR)/lib/libresolv*.so* \
		$(TOOLCHAIN_DIR)/lib/libnsl*.so*
endef

define Host/Install
	$(call Host/SetToolchainInfo)
	$(UCLIBC_MAKE) PREFIX= utils
	$(INSTALL_DIR) $(TOOLCHAIN_DIR)/bin
	$(INSTALL_BIN) \
		$(HOST_BUILD_DIR)/utils/ldd \
		$(TOOLCHAIN_DIR)/bin/
	$(INSTALL_DIR) $(TOOLCHAIN_DIR)/sbin
	$(INSTALL_BIN) \
		$(HOST_BUILD_DIR)/utils/ldconfig \
		$(TOOLCHAIN_DIR)/sbin/
endef

define Host/Clean
	rm -rf \
		$(HOST_BUILD_DIR) \
		$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME) \
		$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev
endef

$(eval $(call HostBuild))
