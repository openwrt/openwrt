#############################################################
#
# openwrt tools
#
#############################################################

OPENWRT_SITE=http://openwrt.openbsd-geek.de

# shared library
OPENWRT_SHARED_SOURCE=openwrt-shared.tar.gz
OPENWRT_SHARED_DIR=$(BUILD_DIR)/openwrt-shared
OPENWRT_SHARED_TARGET_BINARY:=usr/lib/libshared.so

$(DL_DIR)/$(OPENWRT_SHARED_SOURCE):
	$(WGET) -P $(DL_DIR) $(OPENWRT_SITE)/$(OPENWRT_SHARED_SOURCE)

$(OPENWRT_SHARED_DIR)/.source: $(DL_DIR)/$(OPENWRT_SHARED_SOURCE)
	zcat $(DL_DIR)/$(OPENWRT_SHARED_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(OPENWRT_SHARED_DIR)/.source

$(TARGET_DIR)/$(OPENWRT_SHARED_TARGET_BINARY): $(OPENWRT_SHARED_DIR)/.source 
	$(MAKE) -C $(OPENWRT_SHARED_DIR) -f Makefile-openwrt \
		INSTALLDIR=$(TARGET_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld STRIP="$(STRIP)" \
		CFLAGS="$(TARGET_CFLAGS) -I. -I../binary/include -Wall -I$(OPENWRT_SRCBASE)/" \
			install

openwrt-shared: $(TARGET_DIR)/$(OPENWRT_SHARED_TARGET_BINARY)

openwrt-shared-clean:
	-$(MAKE) -C $(OPENWRT_SHARED_DIR) clean


# nvram tool
OPENWRT_NVRAM_SOURCE=openwrt-nvram.tar.gz
OPENWRT_NVRAM_DIR=$(BUILD_DIR)/openwrt-nvram
OPENWRT_NVRAM_TARGET_BINARY:=usr/sbin/nvram

$(DL_DIR)/$(OPENWRT_NVRAM_SOURCE):
	$(WGET) -P $(DL_DIR) $(OPENWRT_SITE)/$(OPENWRT_NVRAM_SOURCE)

$(OPENWRT_NVRAM_DIR)/.source: $(DL_DIR)/$(OPENWRT_NVRAM_SOURCE)
	zcat $(DL_DIR)/$(OPENWRT_NVRAM_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(OPENWRT_NVRAM_DIR)/.source

$(TARGET_DIR)/$(OPENWRT_NVRAM_TARGET_BINARY): $(OPENWRT_NVRAM_DIR)/.source
	$(MAKE) -C $(OPENWRT_NVRAM_DIR) INSTALLDIR=$(TARGET_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld STRIP="$(STRIP)" \
		CFLAGS="$(TARGET_CFLAGS) -I. -I../binary/include -Wall -DOPENWRT_NVRAM" \
			install

openwrt-nvram: $(TARGET_DIR)/$(OPENWRT_NVRAM_TARGET_BINARY)

openwrt-nvram-clean:
	-$(MAKE) -C $(OPENWRT_NVRAM_BUILD_DIR) clean

# mtd tool
OPENWRT_MTD_SOURCE=package/openwrt/mtd.c
OPENWRT_MTD_TARGET_BINARY:=sbin/mtd

$(TARGET_DIR)/$(OPENWRT_MTD_TARGET_BINARY): 
		$(TARGET_CC) -o $(TARGET_DIR)/$(OPENWRT_MTD_TARGET_BINARY) $(OPENWRT_MTD_SOURCE)

openwrt-mtd: $(TARGET_DIR)/$(OPENWRT_MTD_TARGET_BINARY)

# wlconf tool
OPENWRT_WLCONF_SOURCE=openwrt-wlconf.tar.gz
OPENWRT_WLCONF_DIR=$(BUILD_DIR)/openwrt-wlconf
OPENWRT_WLCONF_TARGET_BINARY:=usr/sbin/wlconf

$(DL_DIR)/$(OPENWRT_WLCONF_SOURCE):
	$(WGET) -P $(DL_DIR) $(OPENWRT_SITE)/$(OPENWRT_WLCONF_SOURCE)

$(OPENWRT_WLCONF_DIR)/.source: $(DL_DIR)/$(OPENWRT_WLCONF_SOURCE)
	zcat $(DL_DIR)/$(OPENWRT_WLCONF_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(OPENWRT_WLCONF_DIR)/.source

$(TARGET_DIR)/$(OPENWRT_WLCONF_TARGET_BINARY): $(OPENWRT_WLCONF_DIR)/.source
	$(MAKE) -C $(OPENWRT_WLCONF_DIR) INSTALLDIR=$(TARGET_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld STRIP="$(STRIP)" \
		CFLAGS="$(TARGET_CFLAGS) -I. -I../binary/include -I$(BUILD_DIR)/openwrt-shared \
			-I$(BUILD_DIR)/openwrt-nvram -Wall -DOPENWRT_WLCONF" \
		LDFLAGS="-lnvram -lshared -L$(BUILD_DIR)/openwrt-shared -L$(BUILD_DIR)/openwrt-nvram" \
			install

openwrt-wlconf: $(TARGET_DIR)/$(OPENWRT_WLCONF_TARGET_BINARY)

openwrt-wlconf-clean:
	-$(MAKE) -C $(OPENWRT_WLCONF_BUILD_DIR) clean


openwrt:	openwrt-shared openwrt-nvram openwrt-mtd openwrt-wlconf


