#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kselftests-net
PKG_VERSION:=$(LINUX_VERSION)
PKG_RELEASE:=1

PKG_MAINTAINER:=Paul Spooren <mail@aparcar.org>
PKG_LICENSE:=GPL-2.0-only
# relative to PKG_BUILD_DIR: the kernel's COPYING at the top of KSFT_TOP
PKG_LICENSE_FILES:=../../../COPYING
PKG_FLAGS:=nonshared

# The selftests come from the kernel sources the image is built with, so they
# always match the running kernel. They are copied to tools/testing/selftests
# below KSFT_TOP and patched there; the rest of KSFT_TOP links to the kernel
# tree, which the selftests' install reaches into (scripts/, tools/net/ynl,
# Documentation/netlink).
KSFT_TOP:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)
PKG_BUILD_DIR:=$(KSFT_TOP)/tools/testing/selftests
PATCH_DIR:=./patches-$(KERNEL_PATCHVER)

include $(INCLUDE_DIR)/package.mk

KSFT_TARGETS:=net/forwarding net/lib drivers/net drivers/net/hw drivers/net/dsa

# DEFAULT:=n: opt-in only, so ALL_NONSHARED builds (buildbots) skip it and
# its feed dependencies

define Package/kselftests-net
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=Linux kernel networking selftests
  URL:=https://docs.kernel.org/dev-tools/kselftest.html
  DEFAULT:=n
  DEPENDS:=@!IN_SDK +bash +coreutils-readlink +coreutils-timeout +ip-full +ip-bridge +tc-full \
	+devlink +ethtool-full +jq +mausezahn +ncat +iputils-ping +tcpdump +socat \
	+bc +dc +coreutils-dd +smcroute \
	+iperf3 +python3-light +python3-yaml +python3-ctypes +python3-logging \
	+python3-multiprocessing +python3-uuid \
	+kmod-veth +kmod-vrf +kmod-dummy +kmod-macvlan +kmod-vxlan +kmod-gre \
	+kmod-gre6 +kmod-ipip +kmod-ip6-tunnel +kmod-mpls +kmod-sched-core \
	+kmod-sched +kmod-sched-flower +kmod-sched-act-vlan +kmod-sched-act-pedit \
	+kmod-sched-act-ct +kmod-sched-act-tunnel-key \
	+kmod-sched-act-police +kmod-sched-ets \
	+kmod-sched-prio +kmod-sched-red +kmod-sched-mqprio +kmod-sched-taprio
endef

define Package/kselftests-net/description
  Kernel selftests for networking (net/forwarding, drivers/net,
  drivers/net/hw, drivers/net/dsa) in the layout of `make install` of
  tools/testing/selftests, under /usr/libexec/kselftests-net. Run them
  with run_kselftest.sh -c <collection> or -t <collection>:<test>.
endef

define Build/Prepare
	$(foreach f,$(filter-out tools,$(notdir $(wildcard $(LINUX_DIR)/*))), \
		ln -sfn $(LINUX_DIR)/$(f) $(KSFT_TOP)/$(f);)
	$(foreach f,$(filter-out testing,$(notdir $(wildcard $(LINUX_DIR)/tools/*))), \
		ln -sfn $(LINUX_DIR)/tools/$(f) $(KSFT_TOP)/tools/$(f);)
	$(CP) $(LINUX_DIR)/tools/testing/selftests/. $(PKG_BUILD_DIR)/
	$(Build/Patch)
endef

# shortcut: no BPF objects, no YNL C tools (psp_responder, ncdevmem), no
# iou-zcrx and no csum (needs glibc's error.h); tests needing those fail or skip
KSFT_HELPERS:=drivers/net/napi_id_helper net/lib/xdp_helper

define Build/Compile
	rm -rf $(PKG_BUILD_DIR)/install
	+$(MAKE) $(if $(findstring c,$(OPENWRT_VERBOSE)),V=1,-s --no-print-directory) \
		-C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" CROSS_COMPILE="$(TARGET_CROSS)" CC="$(TARGET_CC)" \
		TARGETS="$(KSFT_TARGETS)" \
		INSTALL_PATH=$(PKG_BUILD_DIR)/install \
		TEST_GEN_FILES= YNL_GEN_FILES= YNL_GENS= \
		install
	$(foreach h,$(KSFT_HELPERS), \
		$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) \
			-I$(PKG_BUILD_DIR) -I$(PKG_BUILD_DIR)/net/lib \
			-o $(PKG_BUILD_DIR)/install/$(h) $(PKG_BUILD_DIR)/$(h).c \
			$(TARGET_LDFLAGS);)
endef

define Package/kselftests-net/install
	$(INSTALL_DIR) $(1)/usr/libexec/$(PKG_NAME)
	$(CP) $(PKG_BUILD_DIR)/install/. $(1)/usr/libexec/$(PKG_NAME)/
endef

$(eval $(call BuildPackage,kselftests-net))
