# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := ppp
PKG_VERSION := 2.4.3
PKG_RELEASE := 3
PKG_MD5SUM := 848f6c3cafeb6074ffeb293c3af79b7c

PKG_SOURCE_SITE := ftp://ftp.samba.org/pub/ppp/
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)

PKG_PPP := $(PACKAGE_DIR)/ppp_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_PPP_PPPOE_PLUGIN := $(PACKAGE_DIR)/ppp-pppoe-plugin_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_PPP_RADIUS_PLUGIN := $(PACKAGE_DIR)/ppp-radius-plugin_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_PPPSTATS := $(PACKAGE_DIR)/pppstats_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_PPPDUMP := $(PACKAGE_DIR)/pppdump_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk

I_PPP := $(PKG_BUILD_DIR)/ipkg/ppp
I_PPP_PPPOE_PLUGIN := $(PKG_BUILD_DIR)/ipkg/ppp-pppoe-plugin
I_PPP_RADIUS_PLUGIN := $(PKG_BUILD_DIR)/ipkg/ppp-radius-plugin
I_PPPDUMP := $(PKG_BUILD_DIR)/ipkg/pppdump
I_PPPSTATS := $(PKG_BUILD_DIR)/ipkg/pppstats

TARGETS := $(PKG_PPP)
ifneq ($(BR2_PACKAGE_PPP_PPPOE),)
TARGETS += $(PKG_PPP_PPPOE_PLUGIN)
endif
ifneq ($(BR2_PACKAGE_PPP_RADIUS),)
TARGETS += $(PKG_PPP_RADIUS_PLUGIN)
endif
ifneq ($(BR2_PACKAGE_PPPDUMP),)
TARGETS += $(PKG_PPPDUMP)
endif
ifneq ($(BR2_PACKAGE_PPPSTATS),)
TARGETS += $(PKG_PPPSTATS)
endif

INSTALL_TARGETS := $(IPKG_STATE_DIR)/info/ppp.list
ifeq ($(BR2_PACKAGE_PPP_PPPOE),y)
INSTALL_TARGETS += $(IPKG_STATE_DIR)/info/ppp-pppoe-plugin.list
endif
ifeq ($(BR2_PACKAGE_PPP_RADIUS),y)
INSTALL_TARGETS += $(IPKG_STATE_DIR)/info/ppp-radius-plugin.list
endif
ifeq ($(BR2_PACKAGE_PPPDUMP),y)
INSTALL_TARGETS += $(IPKG_STATE_DIR)/info/pppdump.list
endif
ifeq ($(BR2_PACKAGE_PPPSTATS),y)
INSTALL_TARGETS += $(IPKG_STATE_DIR)/info/pppstats.list
endif

$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) . $(PKG_NAME).patch
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.patched
	(cd $(PKG_BUILD_DIR); \
	rm -f config.cache; \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS)" \
	./configure \
	 --target=$(GNU_TARGET_NAME) \
	 --host=$(GNU_TARGET_NAME) \
	 --build=$(GNU_HOST_NAME) \
	 --prefix=/usr \
	 --exec-prefix=/usr \
	 --bindir=/usr/bin \
	 --datadir=/usr/share \
	 --infodir=/usr/share/info \
	 --libexecdir=/usr/lib \
	 --localstatedir=/var \
	 --mandir=/usr/share/man \
	 --sbindir=/usr/sbin \
	 --sysconfdir=/etc \
	 $(DISABLE_NLS))
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/pppd/pppd $(PKG_BUILD_DIR)/pppd/plugins/radius/radius.so: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  CC=$(TARGET_CC) \
	  COPTS="$(TARGET_CFLAGS)" \
	 all

$(PKG_PPP): $(PKG_BUILD_DIR)/pppd/pppd
	mkdir -p $(I_PPP)
	cp -af ./ipkg/ppp/* $(I_PPP)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(I_PPP) control/ppp.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH) 
	mkdir -p $(I_PPP)/usr/sbin
	cp -af $(PKG_BUILD_DIR)/pppd/pppd $(I_PPP)/usr/sbin/
	$(STRIP) $(I_PPP)/usr/sbin/*
	mkdir -p $(I_PPP)/usr/lib/pppd
	cp $(PKG_BUILD_DIR)/pppd/plugins/*.so $(I_PPP)/usr/lib/pppd/
	$(STRIP) $(I_PPP)/usr/lib/pppd/*.so
	mkdir -p $(I_PPP)/$(MODULES_SUBDIR)
	cp $(MODULES_DIR)/kernel/drivers/net/ppp_async.o $(I_PPP)/$(MODULES_SUBDIR)
	cp $(MODULES_DIR)/kernel/drivers/net/ppp_deflate.o $(I_PPP)/$(MODULES_SUBDIR)
	cp $(MODULES_DIR)/kernel/drivers/net/bsd_comp.o $(I_PPP)/$(MODULES_SUBDIR)
	find $(I_PPP) -name CVS | xargs rm -rf
	$(IPKG_BUILD) $(I_PPP) $(PACKAGE_DIR)
	
$(IPKG_STATE_DIR)/info/ppp.list: $(PKG_PPP)
	$(IPKG) install $(PKG_PPP)

$(PKG_PPP_PPPOE_PLUGIN): $(PKG_PPP)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(I_PPP_PPPOE_PLUGIN) control/ppp-pppoe-plugin.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(I_PPP_PPPOE_PLUGIN)/usr/lib/pppd/rp-pppoe
	cp -af $(PKG_BUILD_DIR)/pppd/plugins/rp-pppoe/rp-pppoe.so \
		$(I_PPP_PPPOE_PLUGIN)/usr/lib/pppd/rp-pppoe
	$(STRIP) $(I_PPP_PPPOE_PLUGIN)/usr/lib/pppd/*/*.so
	mkdir -p $(I_PPP_PPPOE_PLUGIN)/$(MODULES_SUBDIR)
	cp $(MODULES_DIR)/kernel/drivers/net/pppo*.o $(I_PPP_PPPOE_PLUGIN)/$(MODULES_SUBDIR)
	find $(I_PPP_PPPOE_PLUGIN) -name CVS | xargs rm -rf
	$(IPKG_BUILD) $(I_PPP_PPPOE_PLUGIN) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/ppp-pppoe-plugin.list: $(PKG_PPP_PPPOE_PLUGIN)
	$(IPKG) install $(PKG_PPP_PPPOE_PLUGIN)

$(PKG_PPP_RADIUS_PLUGIN): $(PKG_PPP)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(I_PPP_RADIUS_PLUGIN) control/ppp-radius-plugin.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(I_PPP_RADIUS_PLUGIN)/usr/lib/pppd/radius
	cp -af $(PKG_BUILD_DIR)/pppd/plugins/radius/radius.so \
	  $(I_PPP_RADIUS_PLUGIN)/usr/lib/pppd/radius/
	$(STRIP) $(I_PPP_RADIUS_PLUGIN)/usr/lib/pppd/*/*.so
	find $(I_PPP_RADIUS_PLUGIN) -name CVS | xargs rm -rf
	$(IPKG_BUILD) $(I_PPP_RADIUS_PLUGIN) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/ppp-radius-plugin.list: $(PKG_PPP_RADIUS_PLUGIN)
	$(IPKG) install $(PKG_PPP_RADIUS_PLUGIN)

$(PKG_PPPDUMP): $(PKG_PPP)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(I_PPPDUMP) control/pppdump.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(I_PPPDUMP)/usr/sbin
	cp -af $(PKG_BUILD_DIR)/pppdump/pppdump $(I_PPPDUMP)/usr/sbin/
	$(STRIP) $(I_PPPDUMP)/usr/sbin/*
	find $(I_PPPDUMP) -name CVS | xargs rm -rf
	$(IPKG_BUILD) $(I_PPPDUMP) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/pppdump.list: $(PKG_PPPDUMP)
	$(IPKG) install $(PKG_PPPDUMP)

$(PKG_PPPSTATS): $(PKG_PPP)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(I_PPPSTATS) control/pppstats.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(I_PPPSTATS)/usr/sbin
	cp -af $(PKG_BUILD_DIR)/pppstats/pppstats $(I_PPPSTATS)/usr/sbin/
	$(STRIP) $(I_PPPSTATS)/usr/sbin/*
	find $(I_PPPSTATS) -name CVS | xargs rm -rf
	$(IPKG_BUILD) $(I_PPPSTATS) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/pppstats.list: $(PKG_PPPSTATS)
	$(IPKG) install $(PKG_PPPSTATS)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(TARGETS)
install: $(INSTALL_TARGETS)

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_PPP) $(PKG_PPP_PPPOE_PLUGIN) $(PKG_PPP_RADIUS_PLUGIN) $(PKG_PPPSTATS) $(PKG_PPPDUMP)
