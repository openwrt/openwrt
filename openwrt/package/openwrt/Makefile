# $Id$

include $(TOPDIR)/rules.mk

SHARED_INCLUDE:=${shell pwd}/include

libshared-compile libnvram-compile wlc-compile:
	@$(MAKE) -C $(patsubst %-compile,%,$@) compile
	
libshared-install libnvram-install wlc-install:
	@$(MAKE) -C $(patsubst %-install,%,$@) install

libshared-clean libnvram-clean wlc-clean:
	@$(MAKE) -C $(patsubst %-clean,%,$@) clean

$(TARGET_DIR)/sbin/mtd: mtd.c
	$(TARGET_CC) -o $@ $<
	$(STRIP) $@

$(TARGET_DIR)/sbin/jffs2root: jffs2root.c
	$(TARGET_CC) -o $@ $<
	$(STRIP) $@

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

source:
prepare: 
compile: libnvram-compile libshared-compile wlc-compile
install: libnvram-install libshared-install wlc-install \
	$(TARGET_DIR)/sbin/mtd $(TARGET_DIR)/sbin/jffs2root $(TARGET_DIR)/usr/sbin/wlconf

clean: libnvram-clean libshared-clean wlc-clean
	rm -rf $(LINKSYS_WLCONF_DIR)
	rm -f $(TARGET_DIR)/sbin/mtd
	rm -f $(TARGET_DIR)/sbin/jffs2root
