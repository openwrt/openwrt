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

PKG_BUILD_FLAGS:=no-lto
PKG_BUILD_PARALLEL:=1
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/kselftests-bpf
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:= \
	+libelf +zlib +libpthread +librt @!IN_SDK \
	@KERNEL_DEBUG_FS @KERNEL_DEBUG_INFO_BTF @KERNEL_BPF_EVENTS
  TITLE:=Linux Kernel Selftests (BPF)
  URL:=http://www.kernel.org
endef

define Package/kselftests-bpf/description
  kselftests-bpf is the Linux kernel BPF test suite
endef

EXE_TARGETS = test_verifier

MOD_TARGETS = $(if $(call kernel_patchver_ge,6.4),bpf_testmod.ko)

MAKE_PATH:=tools/testing/selftests/bpf

MAKE_VARS = \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	EXTRA_CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
	LDLIBS="$(TARGET_LDFLAGS)" \
	TOOLCHAIN_INCLUDE="$(TOOLCHAIN_INC_DIRS)" \
	KBUILD_OUTPUT="$(LINUX_DIR)"

MAKE_FLAGS = \
	$(if $(findstring c,$(OPENWRT_VERBOSE)),V=1,V='') \
	OUTPUT=$(PKG_BUILD_DIR)

define Build/Compile
	+$(MAKE_VARS) \
	$(MAKE) $(PKG_JOBS) -C $(LINUX_DIR)/$(MAKE_PATH) \
		$(MAKE_FLAGS) $(EXE_TARGETS) $(MOD_TARGETS) ;
endef

define Package/kselftests-bpf/install
	$(INSTALL_DIR) $(1)/usr/libexec/$(PKG_NAME)
	$(foreach tgt,$(MOD_TARGETS), \
		$(INSTALL_DATA) \
			$(PKG_BUILD_DIR)/$(tgt) $(1)/usr/libexec/$(PKG_NAME); \
	)
	$(foreach tgt,$(EXE_TARGETS), \
		$(INSTALL_BIN) \
			$(PKG_BUILD_DIR)/$(tgt) $(1)/usr/libexec/$(PKG_NAME); \
)
endef

$(eval $(call BuildPackage,kselftests-bpf))
