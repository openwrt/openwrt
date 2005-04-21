# $Id$

include $(TOPDIR)/rules.mk
include kernelconfig.mk

PKG_NAME := iptables
PKG_VERSION := 1.3.1
PKG_RELEASE := 1
PKG_MD5SUM := c3358a3bd0d7755df0b64a5063db296b

PKG_SOURCE_SITE := http://www.netfilter.org/files
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_CAT := bzcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)

PKG_IPT := $(PACKAGE_DIR)/iptables_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPT_UTILS := $(PACKAGE_DIR)/iptables-utils_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPT_EXTRA := $(PACKAGE_DIR)/iptables-extra_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IP6T := $(PACKAGE_DIR)/ip6tables_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk

I_IPT := $(PKG_BUILD_DIR)/ipkg/iptables
I_IPT_UTILS := $(PKG_BUILD_DIR)/ipkg/iptables-utils
I_IPT_EXTRA := $(PKG_BUILD_DIR)/ipkg/iptables-extra
I_IP6T := $(PKG_BUILD_DIR)/ipkg/ip6tables

TARGETS := $(STAGING_DIR)/libipq/libipq.a $(PKG_IPT)
ifneq ($(BR2_PACKAGE_IPTABLES_UTILS),)
TARGETS += $(PKG_IPT_UTILS)
endif
ifneq ($(BR2_PACKAGE_IPTABLES_EXTRA),)
TARGETS += $(PKG_IPT_EXTRA)
endif
ifneq ($(BR2_PACKAGE_IP6TABLES),)
TARGETS += $(PKG_IP6T)
endif

INSTALL_TARGETS := $(IPKG_STATE_DIR)/info/iptables.list
ifeq ($(BR2_PACKAGE_IPTABLES_UTILS),y)
INSTALL_TARGETS += $(IPKG_STATE_DIR)/info/iptables-utils.list
endif
ifeq ($(BR2_PACKAGE_IPTABLES_EXTRA),y)
INSTALL_TARGETS += $(IPKG_STATE_DIR)/info/iptables-extra.list
endif
ifeq ($(BR2_PACKAGE_IP6TABLES),y)
INSTALL_TARGETS += $(IPKG_STATE_DIR)/info/ip6tables.list
endif

$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(PKG_BUILD_DIR)/modules
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/iptables: $(PKG_BUILD_DIR)/.patched
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		KERNEL_DIR=$(LINUX_DIR) PREFIX=/usr \
		CC=$(TARGET_CC) COPT_FLAGS="$(TARGET_CFLAGS)"
		
$(STAGING_DIR)/libipq/libipq.a: $(PKG_BUILD_DIR)/iptables
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		KERNEL_DIR=$(LINUX_DIR) PREFIX=/usr \
		CC=$(TARGET_CC) COPT_FLAGS="$(TARGET_CFLAGS)" \
		libipq/libipq.a
	cp -a $(PKG_BUILD_DIR)/include/* $(STAGING_DIR)/include/
	cp $(PKG_BUILD_DIR)/libipq/libipq.a $(STAGING_DIR)/lib/
	cp $(PKG_BUILD_DIR)/libiptc/libiptc.a $(STAGING_DIR)/lib/

$(PKG_IPT): $(PKG_BUILD_DIR)/iptables
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(I_IPT) control/iptables.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH) 
	mkdir -p $(I_IPT)/usr/sbin
	cp -af $(PKG_BUILD_DIR)/iptables $(I_IPT)/usr/sbin/
	$(STRIP) $(I_IPT)/usr/sbin/iptables
	mkdir -p $(I_IPT)/usr/lib/iptables
	(cd $(PKG_BUILD_DIR)/extensions; \
	 cp $(patsubst %,libipt_%.so,$(ext-y)) $(I_IPT)/usr/lib/iptables)
	-$(STRIP) $(I_IPT)/usr/lib/iptables/*.so
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(I_IPT) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/iptables.list: $(PKG_IPT)
	$(IPKG) install $(PKG_IPT)

$(PKG_IPT_EXTRA): $(PKG_BUILD_DIR)/iptables
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(I_IPT_EXTRA) control/iptables-extra.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH) 
	mkdir -p $(I_IPT_EXTRA)/usr/lib/iptables
	(cd $(PKG_BUILD_DIR)/extensions; \
	 cp $(patsubst %,libipt_%.so,$(ext-m)) $(I_IPT_EXTRA)/usr/lib/iptables)
	-$(STRIP) $(I_IPT_EXTRA)/usr/lib/iptables/*.so
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(I_IPT_EXTRA) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/iptables-extra.list: $(PKG_IPT)
	$(IPKG) install $(PKG_IPT_EXTRA)

$(PKG_IPT_UTILS): $(PKG_BUILD_DIR)/iptables
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(I_IPT_UTILS) control/iptables-utils.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH) 
	mkdir -p $(I_IPT_UTILS)/usr/sbin
	cp $(PKG_BUILD_DIR)/iptables-save $(I_IPT_UTILS)/usr/sbin
	cp $(PKG_BUILD_DIR)/iptables-restore $(I_IPT_UTILS)/usr/sbin
	-$(STRIP) $(I_IPT_UTILS)/usr/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(I_IPT_UTILS) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/iptables-utils.list: $(PKG_IPT)
	$(IPKG) install $(PKG_IPT_UTILS)

$(PKG_IP6T): $(PKG_BUILD_DIR)/iptables
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(I_IP6T) control/ip6tables.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH) 
	mkdir -p $(I_IP6T)/usr/sbin
	cp -af $(PKG_BUILD_DIR)/ip6tables $(I_IP6T)/usr/sbin/
	$(STRIP) $(I_IP6T)/usr/sbin/ip6tables
	mkdir -p $(I_IP6T)/usr/lib/iptables
	(cd $(PKG_BUILD_DIR)/extensions; \
	 cp libip6t_*.so $(I_IP6T)/usr/lib/iptables)
	-$(STRIP) $(I_IP6T)/usr/lib/iptables/*.so
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(I_IP6T) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/ip6tables.list: $(PKG_IP6T)
	$(IPKG) install $(PKG_IP6T)


source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(TARGETS)
install: $(INSTALL_TARGETS)

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPT) $(PKG_IPT_EXTRA) $(PKG_IP6T)
