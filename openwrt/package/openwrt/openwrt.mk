#############################################################
#
# linksys and openwrt tools
#
#############################################################

LINKSYS_KERNEL_INCLUDE=$(BUILD_DIR)/linksys-kernel

# shared library stuff extracted from linksys firmware GPL sourcetree
# WRT54GS_3_37_2_1109_US
LINKSYS_SHARED_TGZ=linksys-shared.tar.gz
LINKSYS_SHARED_DIR=$(BUILD_DIR)/linksys-shared
LINKSYS_SHARED_TARGET_BINARY:=usr/lib/libshared.so

$(DL_DIR)/$(LINKSYS_SHARED_TGZ):
	$(WGET) -P $(DL_DIR) $(LINKSYS_TGZ_SITE)/$(LINKSYS_SHARED_TGZ)

$(LINKSYS_SHARED_DIR)/.source: $(DL_DIR)/$(LINKSYS_SHARED_TGZ)
	zcat $(DL_DIR)/$(LINKSYS_SHARED_TGZ) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(LINKSYS_SHARED_DIR)/.source

$(TARGET_DIR)/$(LINKSYS_SHARED_TARGET_BINARY): $(LINKSYS_SHARED_DIR)/.source 
	$(SED) 's/#include.*/#include <cy_conf.h>/g;' $(LINKSYS_SHARED_DIR)/utils.h
	$(MAKE) -C $(LINKSYS_SHARED_DIR) -f Makefile-openwrt \
		INSTALLDIR=$(TARGET_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld STRIP="$(STRIP)" \
		CFLAGS="$(TARGET_CFLAGS) -I$(LINKSYS_KERNEL_INCLUDE)/include \
		-I$(LINKSYS_KERNEL_INCLUDE) -Wall -I." \
			install

linksys-shared: $(TARGET_DIR)/$(LINKSYS_SHARED_TARGET_BINARY)

linksys-shared-clean:
	-$(MAKE) -C $(LINKSYS_SHARED_DIR) clean


# nvram tool extracted from linksys firmware GPL sourcetree
# WRT54GS_3_37_2_1109_US
LINKSYS_NVRAM_TGZ=linksys-nvram.tar.gz
LINKSYS_NVRAM_DIR=$(BUILD_DIR)/linksys-nvram
LINKSYS_NVRAM_TARGET_BINARY:=usr/sbin/nvram

$(DL_DIR)/$(LINKSYS_NVRAM_TGZ):
	$(WGET) -P $(DL_DIR) $(LINKSYS_TGZ_SITE)/$(LINKSYS_NVRAM_TGZ)

$(LINKSYS_NVRAM_DIR)/.source: $(DL_DIR)/$(LINKSYS_NVRAM_TGZ)
	zcat $(DL_DIR)/$(LINKSYS_NVRAM_TGZ) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(LINKSYS_NVRAM_DIR)/.source

$(TARGET_DIR)/$(LINKSYS_NVRAM_TARGET_BINARY): $(LINKSYS_NVRAM_DIR)/.source
	$(MAKE) -C $(LINKSYS_NVRAM_DIR) INSTALLDIR=$(TARGET_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld STRIP="$(STRIP)" \
		CFLAGS="$(TARGET_CFLAGS) -I. -I$(LINKSYS_KERNEL_INCLUDE)/include \
		-I$(LINKSYS_KERNEL_INCLUDE) -I$(LINKSYS_SHARED_DIR) -Wall" install

linksys-nvram: $(TARGET_DIR)/$(LINKSYS_NVRAM_TARGET_BINARY)

linksys-nvram-clean:
	-$(MAKE) -C $(LINKSYS_NVRAM_BUILD_DIR) clean

# wlconf tool extracted from linksys firmware GPL sourcetree
# WRT54GS_3_37_2_1109_US
LINKSYS_WLCONF_TGZ=linksys-wlconf.tar.gz
LINKSYS_WLCONF_DIR=$(BUILD_DIR)/linksys-wlconf
LINKSYS_WLCONF_TARGET_BINARY:=usr/sbin/wlconf

$(DL_DIR)/$(LINKSYS_WLCONF_TGZ):
	$(WGET) -P $(DL_DIR) $(LINKSYS_TGZ_SITE)/$(LINKSYS_WLCONF_TGZ)

$(LINKSYS_WLCONF_DIR)/.source: $(DL_DIR)/$(LINKSYS_WLCONF_TGZ)
	zcat $(DL_DIR)/$(LINKSYS_WLCONF_TGZ) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(LINKSYS_WLCONF_DIR)/.source

$(TARGET_DIR)/$(LINKSYS_WLCONF_TARGET_BINARY): $(LINKSYS_WLCONF_DIR)/.source
	$(MAKE) -C $(LINKSYS_WLCONF_DIR) INSTALLDIR=$(TARGET_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld STRIP="$(STRIP)" \
		CFLAGS="$(TARGET_CFLAGS) -I. -I$(LINKSYS_KERNEL_INCLUDE)/include \
			-I$(LINKSYS_KERNEL_INCLUDE) \
			-I$(LINKSYS_SHARED_DIR) \
			-I$(LINKSYS_NVRAM_DIR) \
			-Wall -DOPENWRT_WLCONF" \
		LDFLAGS="-lnvram -lshared -L$(BUILD_DIR)/linksys-shared -L$(BUILD_DIR)/linksys-nvram" \
			install

linksys-wlconf: $(TARGET_DIR)/$(LINKSYS_WLCONF_TARGET_BINARY)

linksys-wlconf-clean:
	-$(MAKE) -C $(LINKSYS_WLCONF_BUILD_DIR) clean


# mtd tool
OPENWRT_MTD_SOURCE=package/openwrt/mtd.c
OPENWRT_MTD_TARGET_BINARY:=sbin/mtd

$(TARGET_DIR)/$(OPENWRT_MTD_TARGET_BINARY): 
		$(TARGET_CC) -o $(TARGET_DIR)/$(OPENWRT_MTD_TARGET_BINARY) $(OPENWRT_MTD_SOURCE)

openwrt-mtd: $(TARGET_DIR)/$(OPENWRT_MTD_TARGET_BINARY)

# trx tool
OPENWRT_TRX_SOURCE=package/openwrt/trx.c
OPENWRT_TRX_TARGET=$(BUILD_DIR)/trx

$(OPENWRT_TRX_TARGET): 
		$(CC) -o $(OPENWRT_TRX_TARGET) $(OPENWRT_TRX_SOURCE)

openwrt-trx: $(OPENWRT_TRX_TARGET)

# addpattern tool
OPENWRT_ADDPATTERN_SOURCE=package/openwrt/addpattern.c
OPENWRT_ADDPATTERN_TARGET=$(BUILD_DIR)/addpattern

$(OPENWRT_ADDPATTERN_TARGET): 
		$(CC) -o $(OPENWRT_ADDPATTERN_TARGET) $(OPENWRT_ADDPATTERN_SOURCE)

openwrt-addpattern: $(OPENWRT_ADDPATTERN_TARGET)

openwrt:	linksys-shared linksys-nvram linksys-wlconf openwrt-mtd openwrt-trx openwrt-addpattern


