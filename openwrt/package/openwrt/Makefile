# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := openwrt-utils
PKG_RELEASE := 1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

SHARED_INCLUDE:=${shell pwd}/include

libshared-compile libnvram-compile:
	@$(MAKE) -C $(patsubst %-compile,%,$@) compile
	
libshared-install libnvram-install: libshared-compile libnvram-compile
	@$(MAKE) -C $(patsubst %-install,%,$@) install

libshared-clean libnvram-clean wlc-clean:
	@$(MAKE) -C $(patsubst %-clean,%,$@) clean

$(PKG_IPK_DIR)/sbin/mtd: mtd.c
	mkdir -p $(PKG_IPK_DIR)/sbin
	$(TARGET_CC) -o $@ $<

$(PKG_IPK_DIR)/sbin/jffs2root: jffs2root.c
	mkdir -p $(PKG_IPK_DIR)/sbin
	$(TARGET_CC) -o $@ $<

$(PKG_IPK_DIR)/usr/sbin/wlc: wlc.c libshared-install
	mkdir -p $(PKG_IPK_DIR)/usr/sbin
	$(TARGET_CC) -o $@ $< -lshared -L./libshared -I$(SHARED_INCLUDE)

# wlconf tool extracted from linksys firmware GPL.unpackedtree
# WRT54GS_3_37_2_1109_US
LINKSYS_TGZ_SITE=http://openwrt.openbsd-geek.de
LINKSYS_TGZ_MD5SUM=1b596e5bf26d2898d1eab5bd200bee58
LINKSYS_WLCONF_TGZ=linksys-wlconf.tar.gz
LINKSYS_WLCONF_DIR=$(BUILD_DIR)/linksys-wlconf

$(DL_DIR)/$(LINKSYS_WLCONF_TGZ):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(LINKSYS_WLCONF_TGZ) $(LINKSYS_TGZ_MD5SUM) $(LINKSYS_TGZ_SITE)

$(LINKSYS_WLCONF_DIR)/.unpacked: $(DL_DIR)/$(LINKSYS_WLCONF_TGZ)
	zcat $(DL_DIR)/$(LINKSYS_WLCONF_TGZ) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(LINKSYS_WLCONF_DIR)/.unpacked
	
$(TARGET_DIR)/usr/sbin/wlconf: $(LINKSYS_WLCONF_DIR)/.unpacked 
	$(MAKE) -C $(LINKSYS_WLCONF_DIR) INSTALLDIR=$(TARGET_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld STRIP="$(STRIP)" \
		CFLAGS="$(TARGET_CFLAGS) -I. -I$(SHARED_INCLUDE) \
			-Wall -DOPENWRT_WLCONF" \
		LDFLAGS="-lnvram -lshared" \
			install
	$(STRIP) $@

$(PKG_IPK): $(PKG_IPK_DIR)/sbin/mtd $(PKG_IPK_DIR)/sbin/jffs2root $(PKG_IPK_DIR)/usr/sbin/wlc
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_RELEASE) $(ARCH)
	$(STRIP) $(PKG_IPK_DIR)/usr/sbin/*
	$(STRIP) $(PKG_IPK_DIR)/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME).list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(LINKSYS_WLCONF_TGZ)
prepare: $(LINKSYS_WLCONF_DIR)/.unpacked
compile: prepare libnvram-compile libshared-compile $(PKG_IPK)
install: libnvram-install libshared-install \
	 $(TARGET_DIR)/usr/sbin/wlconf \
	 $(IPKG_STATE_DIR)/info/$(PKG_NAME).list

clean: libshared-clean libnvram-clean
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)
