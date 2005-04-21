# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := openwrt-utils
PKG_RELEASE := 1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

PKG_WLCOMPAT := $(PACKAGE_DIR)/kmod-wlcompat_$(LINUX_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk

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
	$(IPKG) install $<

$(IPKG_STATE_DIR)/info/kmod-wlcompat.list: $(PKG_WLCOMPAT)
	$(IPKG) install $<


WLCOMPAT_FLAGS:=$(TARGET_CLFAGS) -D__KERNEL__ -fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0 \
		-mno-abicalls -fno-pic -finline-limit=100000 -mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 \
		-Wa,-mips32 -Wa,--trap -DMODULE -mlong-calls -fno-common -I. -I linux-2.4 -funsigned-char -nostdinc \
		-iwithprefix include -I$(LINUX_DIR)/include -I$(LINUX_DIR)/include/asm/gcc -I./include -c
		
$(PKG_BUILD_DIR)/wlcompat.o:
	$(TARGET_CC) $(WLCOMPAT_FLAGS) -o $@ wlcompat.c

$(PKG_BUILD_DIR)/wlcompat-debug.o:
	$(TARGET_CC) -DDEBUG $(WLCOMPAT_FLAGS) -o $@ wlcompat.c

$(PKG_WLCOMPAT): $(PKG_BUILD_DIR)/wlcompat.o $(PKG_BUILD_DIR)/wlcompat-debug.o
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_BUILD_DIR)/wlcompat kmod-wlcompat.control $(LINUX_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_BUILD_DIR)/wlcompat/etc/modules.d
	echo "insmod wlcompat" > $(PKG_BUILD_DIR)/wlcompat/etc/modules.d/10-wlcompat
	mkdir -p $(PKG_BUILD_DIR)/wlcompat/lib/modules/$(LINUX_VERSION)
	cp $^ $(PKG_BUILD_DIR)/wlcompat/lib/modules/$(LINUX_VERSION)/
	$(IPKG_BUILD) $(PKG_BUILD_DIR)/wlcompat $(PACKAGE_DIR)

source: $(DL_DIR)/$(LINKSYS_WLCONF_TGZ)
prepare: $(LINKSYS_WLCONF_DIR)/.unpacked
compile: prepare libnvram-compile libshared-compile $(PKG_IPK) $(PKG_WLCOMPAT)
install: libnvram-install libshared-install \
	 $(TARGET_DIR)/usr/sbin/wlconf \
	 $(IPKG_STATE_DIR)/info/$(PKG_NAME).list \
	 $(IPKG_STATE_DIR)/info/kmod-wlcompat.list

clean: libshared-clean libnvram-clean
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)
