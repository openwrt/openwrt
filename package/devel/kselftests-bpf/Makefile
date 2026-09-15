#
# Copyright (C) 2021 Tony Ambardar <itugrok@yahoo.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kselftests
PKG_VERSION:=$(LINUX_VERSION)
PKG_RELEASE:=1
PKG_MAINTAINER:=Tony Ambardar <itugrok@yahoo.com>
PKG_LICENSE:=GPL-2.0-only

PKG_BUILD_FLAGS:=no-lto
PKG_BUILD_PARALLEL:=1
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

MAKE_PATH:=tools/testing/selftests

MAKE_VARS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	CC="$(TARGET_CC)" \
	AR="$(TARGET_AR)" \
	LD="$(TARGET_LD)" \
	NM="$(TARGET_NM)" \
	OBJCOPY="$(TARGET_OBJCOPY)" \
	EXTRA_CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
	LDLIBS="$(TARGET_LDFLAGS)" \
	TOOLCHAIN_INCLUDE="$(TOOLCHAIN_INC_DIRS)" \
	KBUILD_OUTPUT="$(LINUX_DIR)"

MAKE_FLAGS:= \
	$(if $(findstring c,$(OPENWRT_VERBOSE)),V=1,V='')

# List of all selftest targets (excluding bpf which has special handling)
KSELFTEST_TARGETS:=size kcmp rtc timers futex exec clone3 openat2 mincore mqueue net sigaltstack splice sync

# Common install template
define KselftestInstall
	$(INSTALL_DIR) $(1)/usr/libexec/kselftests/$(2)
	-$(CP) $(PKG_BUILD_DIR)/$(2)/* $(1)/usr/libexec/kselftests/$(2)/
	-find $(1)/usr/libexec/kselftests/$(2) -type f \( -name "*.c" -o -name "*.h" -o -name "*.o" -o -name "Makefile" \) -delete
	-find $(1)/usr/libexec/kselftests/$(2) -type f -executable -exec $(TARGET_CROSS)strip {} \; 2>/dev/null
endef

# =============================================================================
# BPF Selftests Package
# =============================================================================

define Package/kselftests-bpf
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=+libelf +zlib +libpthread +librt +libpcap @!IN_SDK \
	@KERNEL_DEBUG_FS @KERNEL_DEBUG_INFO_BTF @KERNEL_BPF_EVENTS
  TITLE:=Kernel Selftests - BPF
  URL:=https://www.kernel.org
endef

define Package/kselftests-bpf/description
  Linux kernel BPF test suite including test_verifier and bpf_testmod
endef

BPF_EXE_TARGETS:=test_verifier
BPF_MOD_TARGETS:=bpf_testmod.ko

define Package/kselftests-bpf/install
	$(INSTALL_DIR) $(1)/usr/libexec/kselftests/bpf
	$(foreach tgt,$(BPF_MOD_TARGETS), \
		$(INSTALL_DATA) $(PKG_BUILD_DIR)/bpf/$(tgt) $(1)/usr/libexec/kselftests/bpf/; \
	)
	$(foreach tgt,$(BPF_EXE_TARGETS), \
		$(INSTALL_BIN) $(PKG_BUILD_DIR)/bpf/$(tgt) $(1)/usr/libexec/kselftests/bpf/; \
	)
endef

# =============================================================================
# Generic Selftests Packages
# =============================================================================

define Package/kselftests-size
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=@!IN_SDK
  TITLE:=Kernel Selftests - size
  URL:=https://www.kernel.org
endef

define Package/kselftests-size/description
  Linux kernel selftest: Binary size test
endef

define Package/kselftests-size/install
	$(call KselftestInstall,$(1),size)
endef

define Package/kselftests-kcmp
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=@!IN_SDK +libpthread
  TITLE:=Kernel Selftests - kcmp
  URL:=https://www.kernel.org
endef

define Package/kselftests-kcmp/description
  Linux kernel selftest: Process comparison tests (kcmp syscall)
endef

define Package/kselftests-kcmp/install
	$(call KselftestInstall,$(1),kcmp)
endef

define Package/kselftests-rtc
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=@!IN_SDK +libpthread
  TITLE:=Kernel Selftests - rtc
  URL:=https://www.kernel.org
endef

define Package/kselftests-rtc/description
  Linux kernel selftest: Real-time clock tests
endef

define Package/kselftests-rtc/install
	$(call KselftestInstall,$(1),rtc)
endef

define Package/kselftests-timers
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=@!IN_SDK +libpthread +librt
  TITLE:=Kernel Selftests - timers
  URL:=https://www.kernel.org
endef

define Package/kselftests-timers/description
  Linux kernel selftest: Timer subsystem tests
endef

define Package/kselftests-timers/install
	$(call KselftestInstall,$(1),timers)
endef

define Package/kselftests-futex
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=@!IN_SDK +libpthread
  TITLE:=Kernel Selftests - futex
  URL:=https://www.kernel.org
endef

define Package/kselftests-futex/description
  Linux kernel selftest: Futex tests
endef

define Package/kselftests-futex/install
	$(call KselftestInstall,$(1),futex)
endef

define Package/kselftests-exec
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=@!IN_SDK +libpthread
  TITLE:=Kernel Selftests - exec
  URL:=https://www.kernel.org
endef

define Package/kselftests-exec/description
  Linux kernel selftest: Program execution tests
endef

define Package/kselftests-exec/install
	$(call KselftestInstall,$(1),exec)
endef

define Package/kselftests-clone3
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=@!IN_SDK +libpthread
  TITLE:=Kernel Selftests - clone3
  URL:=https://www.kernel.org
endef

define Package/kselftests-clone3/description
  Linux kernel selftest: clone3 syscall tests
endef

define Package/kselftests-clone3/install
	$(call KselftestInstall,$(1),clone3)
endef

define Package/kselftests-openat2
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=@!IN_SDK
  TITLE:=Kernel Selftests - openat2
  URL:=https://www.kernel.org
endef

define Package/kselftests-openat2/description
  Linux kernel selftest: openat2 syscall tests
endef

define Package/kselftests-openat2/install
	$(call KselftestInstall,$(1),openat2)
endef

define Package/kselftests-mincore
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=@!IN_SDK
  TITLE:=Kernel Selftests - mincore
  URL:=https://www.kernel.org
endef

define Package/kselftests-mincore/description
  Linux kernel selftest: mincore syscall tests
endef

define Package/kselftests-mincore/install
	$(call KselftestInstall,$(1),mincore)
endef

define Package/kselftests-mqueue
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=@!IN_SDK +libpthread +librt
  TITLE:=Kernel Selftests - mqueue
  URL:=https://www.kernel.org
endef

define Package/kselftests-mqueue/description
  Linux kernel selftest: POSIX message queue tests
endef

define Package/kselftests-mqueue/install
	$(call KselftestInstall,$(1),mqueue)
endef

define Package/kselftests-net
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=@!IN_SDK +libpthread
  TITLE:=Kernel Selftests - net
  URL:=https://www.kernel.org
endef

define Package/kselftests-net/description
  Linux kernel selftest: Networking stack tests
endef

define Package/kselftests-net/install
	$(call KselftestInstall,$(1),net)
endef

define Package/kselftests-sigaltstack
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=@!IN_SDK
  TITLE:=Kernel Selftests - sigaltstack
  URL:=https://www.kernel.org
endef

define Package/kselftests-sigaltstack/description
  Linux kernel selftest: Signal alternate stack tests
endef

define Package/kselftests-sigaltstack/install
	$(call KselftestInstall,$(1),sigaltstack)
endef

define Package/kselftests-splice
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=@!IN_SDK
  TITLE:=Kernel Selftests - splice
  URL:=https://www.kernel.org
endef

define Package/kselftests-splice/description
  Linux kernel selftest: splice syscall tests
endef

define Package/kselftests-splice/install
	$(call KselftestInstall,$(1),splice)
endef

define Package/kselftests-sync
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=@!IN_SDK
  TITLE:=Kernel Selftests - sync
  URL:=https://www.kernel.org
endef

define Package/kselftests-sync/description
  Linux kernel selftest: sync_file_range tests
endef

define Package/kselftests-sync/install
	$(call KselftestInstall,$(1),sync)
endef

# =============================================================================
# Build Logic
# =============================================================================

define Build/Compile
	$(if $(CONFIG_PACKAGE_kselftests-bpf), \
		mkdir -p $(PKG_BUILD_DIR)/bpf; \
		+$(MAKE_VARS) $(MAKE) $(PKG_JOBS) -C $(LINUX_DIR)/$(MAKE_PATH)/bpf \
			$(MAKE_FLAGS) OUTPUT=$(PKG_BUILD_DIR)/bpf/ \
			$(BPF_EXE_TARGETS) $(BPF_MOD_TARGETS); \
	)
	$(foreach target,$(KSELFTEST_TARGETS), \
		$(if $(CONFIG_PACKAGE_kselftests-$(target)), \
			mkdir -p $(PKG_BUILD_DIR)/$(target); \
			+$(MAKE_VARS) $(MAKE) $(PKG_JOBS) -C $(LINUX_DIR)/$(MAKE_PATH)/$(target) \
				$(MAKE_FLAGS) OUTPUT="$(PKG_BUILD_DIR)/$(target)/" all || true; \
		) \
	)
endef

# =============================================================================
# Package Evaluation
# =============================================================================

$(eval $(call BuildPackage,kselftests-bpf))
$(eval $(call BuildPackage,kselftests-size))
$(eval $(call BuildPackage,kselftests-kcmp))
$(eval $(call BuildPackage,kselftests-rtc))
$(eval $(call BuildPackage,kselftests-timers))
$(eval $(call BuildPackage,kselftests-futex))
$(eval $(call BuildPackage,kselftests-exec))
$(eval $(call BuildPackage,kselftests-clone3))
$(eval $(call BuildPackage,kselftests-openat2))
$(eval $(call BuildPackage,kselftests-mincore))
$(eval $(call BuildPackage,kselftests-mqueue))
$(eval $(call BuildPackage,kselftests-net))
$(eval $(call BuildPackage,kselftests-sigaltstack))
$(eval $(call BuildPackage,kselftests-splice))
$(eval $(call BuildPackage,kselftests-sync))
