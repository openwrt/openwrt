#
# Copyright (C) 2021 Tony Ambardar <itugrok@yahoo.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kselftests-bpf
PKG_VERSION:=$(LINUX_VERSION)
PKG_RELEASE:=1
PKG_MAINTAINER:=Tony Ambardar <itugrok@yahoo.com>

PKG_BUILD_FLAGS:=gc-sections lto
PKG_BUILD_PARALLEL:=1
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/kselftests-bpf
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:= +libelf +zlib +libpthread +librt @!IN_SDK \
	    @KERNEL_DEBUG_FS @KERNEL_DEBUG_INFO_BTF @KERNEL_BPF_EVENTS
  TITLE:=Linux Kernel Selftests (BPF)
  URL:=http://www.kernel.org
endef

define Package/kselftests-bpf/description
  kselftests-bpf is the Linux kernel BPF test suite
endef

TEST_TARGET = test_verifier

MAKE_PATH:=tools/testing/selftests/bpf

MAKE_VARS = \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SAN_CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
	LDLIBS="$(TARGET_LDFLAGS)" \
	TOOLCHAIN_INCLUDE="$(TOOLCHAIN_DIR)/include" \
	VMLINUX_BTF="$(LINUX_DIR)/vmlinux"

MAKE_FLAGS = \
	$(if $(findstring c,$(OPENWRT_VERBOSE)),V=1,V='') \
	O=$(PKG_BUILD_DIR)

define Build/Compile
	+$(MAKE_VARS) \
	$(MAKE) $(PKG_JOBS) -C $(LINUX_DIR)/$(MAKE_PATH) \
		$(MAKE_FLAGS) $(TEST_TARGET) ;
endef

define Package/kselftests-bpf/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(TEST_TARGET) $(1)/usr/bin/
endef

$(eval $(call BuildPackage,kselftests-bpf))
