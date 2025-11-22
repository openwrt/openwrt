include $(TOPDIR)/rules.mk

PKG_NAME:=xdp-tools
PKG_RELEASE:=1
PKG_VERSION:=1.5.8
PKG_HASH:=e7b9717074c511fef34aab6e22b16af55fb33307505a5785d25a82b542e596ae

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/xdp-project/xdp-tools/tar.gz/v$(PKG_VERSION)?
PKG_ABI_VERSION:=$(call abi_version_str,$(PKG_VERSION))

PKG_MAINTAINER:=Daniel Golle <daniel@makrotopia.org>

PKG_BUILD_DEPENDS:=bpf-headers bpftool/host

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/bpf.mk
include $(INCLUDE_DIR)/nls.mk

PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

define Package/libxdp
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libxdp - Library for use with XDP
  LICENSE:=LGPL-2.1 OR BSD-2-Clause
  ABI_VERSION:=$(PKG_ABI_VERSION)
  URL:=https://github.com/xdp-project/xdp-tools/
  DEPENDS:=+libbpf $(BPF_DEPENDS)
endef

define Package/libxdp/description
libxdp - library for attaching XDP programs and using AF_XDP sockets
endef

define Package/xdp-tools/Default
  SECTION:=net
  CATEGORY:=Network
  LICENSE:=GPL-2.0-only
  URL:=https://github.com/xdp-project/xdp-tools/
  DEPENDS:=+libxdp
endef


define Package/xdp-bench
$(call Package/xdp-tools/Default)
  TITLE:=xdp-bench - an XDP benchmarking tool
  DEPENDS+=@KERNEL_DEBUG_INFO_BTF @KERNEL_KPROBES
endef

define Package/xdp-bench/description
XDP-bench is a benchmarking utility for exercising the different operation modes
of XDP. It is intended to be a simple program demonstrating the various operating
modes; these include dropping packets, hairpin forwarding (using the XDP_TX return
code), and redirection using the various in-kernel packet redirection facilities.
endef


define Package/xdp-filter
$(call Package/xdp-tools/Default)
  TITLE:=xdp-filter - a simple XDP-powered packet filter
endef

define Package/xdp-filter/description
xdp-filter is a packet filtering utility powered by XDP. It is deliberately
simple and so does not have the same matching capabilities as, e.g.,
netfilter. Instead, thanks to XDP, it can achieve very high drop rates:
tens of millions of packets per second on a single CPU core.
endef


define Package/xdp-forward
$(call Package/xdp-tools/Default)
  TITLE:=xdp-forward - the XDP forwarding plane
  DEPENDS+=@KERNEL_DEBUG_INFO_BTF
endef

define Package/xdp-forward/description
xdp-forward is an XDP forwarding plane, which will accelerate
packet forwarding using XDP. To use it, simply load it on
the set of interfaces to accelerate forwarding between.

The userspace component of xdp-forward will then configure and
load XDP programs on those interfaces, and forward packets between
them using XDP_REDIRECT, using the kernel routing table or
netfilter flowtable to determine the destination for each packet.
endef


define Package/xdp-loader
$(call Package/xdp-tools/Default)
  TITLE:=xdp-loader - an XDP program loader
endef

define Package/xdp-loader/description
xdp-loader is a simple loader for XDP programs with support for attaching
multiple programs to the same interface. To achieve this it exposes the same
load and unload semantics exposed by the libxdp library.
endef


define Package/xdp-monitor
$(call Package/xdp-tools/Default)
  TITLE:=xdp-monitor - a simple XDP tracepoint monitoring tool
  DEPENDS+=@KERNEL_DEBUG_INFO_BTF @KERNEL_KPROBES
endef

define Package/xdp-monitor/description
XDP-monitor is a tool that monitors various XDP related statistics and
events using BPF tracepoints infrastructure, trying to be as low overhead
as possible.
endef


define Package/xdp-trafficgen
$(call Package/xdp-tools/Default)
  TITLE:=xdp-trafficgen - an XDP-based packet generator
  DEPENDS+=@KERNEL_DEBUG_INFO_BTF @KERNEL_KPROBES
endef

define Package/xdp-trafficgen/description
XDP-trafficgen is a packet generator utilising the XDP kernel subsystem
to generate packets transmit them through a network interface.
Packets are dynamically generated and transmitted in the kernel,
allowing for high performance (millions of packets per second per core).

XDP-trafficgen supports generating UDP traffic with fixed or dynamic
destination ports, and also has basic support for generating dummy
TCP traffic on a single flow.
endef


define Package/xdpdump
$(call Package/xdp-tools/Default)
  TITLE:=xdpdump - tool for capturing packets at the XDP layer
  DEPENDS+=+libpcap
endef

define Package/xdpdump/description
xdpdump - a simple tcpdump like tool for capturing packets at the XDP layer
endef


TARGET_LDFLAGS += $(INTL_LDFLAGS)

CONFIGURE_VARS += \
	FORCE_SYSTEM_LIBBPF=1 \
	CC="$(TARGET_CC)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	CLANG="$(CLANG)" \
	BPFTOOL="$(STAGING_DIR_HOST)/usr/sbin/bpftool"

ifneq ($(findstring s,$(OPENWRT_VERBOSE)),)
	MAKE_FLAGS+=V=1
endif

MAKE_VARS += \
	PREFIX=/usr \
	RUNDIR=/tmp/run \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)"

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/xdp
	$(CP) $(PKG_INSTALL_DIR)/usr/include/xdp/*.h $(1)/usr/include/xdp/
	$(INSTALL_DIR) $(1)/usr/lib/bpf
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libxdp.{a,so*} $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/bpf/*.o $(1)/usr/lib/bpf
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libxdp.pc \
		$(1)/usr/lib/pkgconfig/
	$(SED) 's,/usr/include,$$$${prefix}/include,g' \
		$(1)/usr/lib/pkgconfig/libxdp.pc
	$(SED) 's,/usr/lib,$$$${exec_prefix}/lib,g' \
		$(1)/usr/lib/pkgconfig/libxdp.pc
endef

define Package/xdp-bench/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/xdp-bench $(1)/usr/sbin
endef

define Package/xdp-filter/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/xdp-filter $(1)/usr/sbin
endef

define Package/xdp-forward/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/xdp-forward $(1)/usr/sbin
endef

define Package/xdp-loader/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/xdp-loader $(1)/usr/sbin
endef

define Package/xdp-monitor/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/xdp-monitor $(1)/usr/sbin
endef

define Package/xdp-trafficgen/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/xdp-trafficgen $(1)/usr/sbin
endef

define Package/xdpdump/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/xdpdump $(1)/usr/sbin
endef

define Package/libxdp/install
	$(INSTALL_DIR) $(1)/usr/lib/bpf
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libxdp.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/bpf/*.o $(1)/usr/lib/bpf
endef

$(eval $(call BuildPackage,libxdp))
$(eval $(call BuildPackage,xdp-bench))
$(eval $(call BuildPackage,xdp-filter))
$(eval $(call BuildPackage,xdp-forward))
$(eval $(call BuildPackage,xdp-loader))
$(eval $(call BuildPackage,xdp-monitor))
$(eval $(call BuildPackage,xdp-trafficgen))
$(eval $(call BuildPackage,xdpdump))
