#############################################################
# $Id$
#
# Linux kernel target for the OpenWRT project
#
# patches are sorted by numbers
# 000	patch between linux-2.4.29 and linux-mips-cvs
# 0xx	linksys patches
# 1xx	OpenWRT patches (diag,compressed,..)
# 2xx	fixes for 2.4.29 integration (wl driver)
# 3xx	kernel feature patches (squashfs,jffs2 compression,..)
# 4xx	patches needed to integrate feature patches
#
#############################################################

LINUX_FORMAT=zImage
LINUX_KARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	)

LINUX_BINARY_DRIVER_SITE=http://openwrt.openbsd-geek.de
LINUX_SOURCE=linux-$(LINUX_VERSION).tar.bz2
LINUX_KCONFIG=./linux.config
LINUX_PATCHES=./kernel-patches
LINUX_KERNEL_SOURCE=./kernel-source
LINUX_BINLOC=arch/$(LINUX_KARCH)/brcm-boards/bcm947xx/compressed/piggy
# Used by pcmcia-cs and others
LINUX_SOURCE_DIR=$(LINUX_DIR)-$(LINUX_VERSION)

# binary driver extracted from linksys firmware GPL sourcetree WRT54GS_3_37_2_1109_US 
LINUX_BINARY_WL_DRIVER=kernel-binary-wl-0.1.tar.gz
LINUX_BINARY_WL_MD5SUM=1b57ba129ad80c7f1702d0be1422cfba
LINUX_BINARY_ET_DRIVER=kernel-binary-et-0.2.tar.gz
LINUX_BINARY_ET_MD5SUM=d657f929bceee926bc28821d753d945c

TARGET_MODULES_DIR=$(TARGET_DIR)/lib/modules/$(LINUX_VERSION)

$(DL_DIR)/$(LINUX_BINARY_WL_DRIVER):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(LINUX_BINARY_WL_DRIVER) $(LINUX_BINARY_WL_MD5SUM) $(LINUX_BINARY_DRIVER_SITE)

$(DL_DIR)/$(LINUX_BINARY_ET_DRIVER):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(LINUX_BINARY_ET_DRIVER) $(LINUX_BINARY_ET_MD5SUM) $(LINUX_BINARY_DRIVER_SITE)

$(LINUX_DIR)/.unpacked: $(DL_DIR)/$(LINUX_SOURCE) $(DL_DIR)/$(LINUX_BINARY_WL_DRIVER) $(DL_DIR)/$(LINUX_BINARY_ET_DRIVER)
	-mkdir -p $(BUILD_DIR)
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	ln -sf $(LINUX_DIR)-$(LINUX_VERSION) $(LINUX_DIR)
	# extract wlan and lan binary only driver
	zcat $(DL_DIR)/$(LINUX_BINARY_WL_DRIVER) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	zcat $(DL_DIR)/$(LINUX_BINARY_ET_DRIVER) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(LINUX_DIR)/.unpacked

$(LINUX_DIR)/.patched: $(LINUX_DIR)/.unpacked
	$(PATCH) $(LINUX_DIR) $(LINUX_PATCHES)
	# copy kernel source which is maintained in openwrt via cvs
	cp -a $(LINUX_KERNEL_SOURCE)/* $(LINUX_DIR)
	# copy binary drivers
	cp -a $(BUILD_DIR)/wl/*.o $(LINUX_DIR)/drivers/net/wl
	cp -a $(BUILD_DIR)/et/*.o $(LINUX_DIR)/drivers/net/et
	touch $(LINUX_DIR)/.patched

$(LINUX_DIR)/.configured: $(LINUX_DIR)/.patched
	-cp $(LINUX_KCONFIG) $(LINUX_DIR)/.config
	$(SED) "s,^CROSS_COMPILE.*,CROSS_COMPILE=$(KERNEL_CROSS),g;" \
	  $(LINUX_DIR)/Makefile  \
	  $(LINUX_DIR)/arch/mips/Makefile
	$(SED) "s,\-mcpu=,\-mtune=,g;" $(LINUX_DIR)/arch/mips/Makefile
	$(MAKE) -C $(LINUX_DIR) ARCH=$(LINUX_KARCH) oldconfig include/linux/version.h
	touch $(LINUX_DIR)/.configured

$(LINUX_DIR)/.depend_done:  $(LINUX_DIR)/.configured
	$(MAKE) -C $(LINUX_DIR) ARCH=$(LINUX_KARCH) dep
	touch $(LINUX_DIR)/.depend_done

$(LINUX_DIR)/$(LINUX_BINLOC): $(LINUX_DIR)/.depend_done
	$(MAKE) -C $(LINUX_DIR) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) $(LINUX_FORMAT)
	$(MAKE) -C $(LINUX_DIR) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) modules

$(LINUX_KERNEL): $(LINUX_DIR)/$(LINUX_BINLOC)
	cp -fa $(LINUX_DIR)/$(LINUX_BINLOC) $(LINUX_KERNEL)
	touch -c $(LINUX_KERNEL)

$(LINUX_DIR)/.modules_done: $(LINUX_KERNEL)
	rm -rf $(BUILD_DIR)/modules
	$(MAKE) -C $(LINUX_DIR) DEPMOD=true INSTALL_MOD_PATH=$(BUILD_DIR)/modules modules_install
	touch $(LINUX_DIR)/.modules_done

$(STAGING_DIR)/include/linux/version.h: $(LINUX_DIR)/.configured
	mkdir -p $(STAGING_DIR)/include
	tar -ch -C $(LINUX_DIR)/include -f - linux | tar -xf - -C $(STAGING_DIR)/include/
	tar -ch -C $(LINUX_DIR)/include -f - asm | tar -xf - -C $(STAGING_DIR)/include/

$(TARGET_MODULES_DIR): 
	-mkdir -p $(TARGET_MODULES_DIR)
	cp $(LINUX_DIR)/drivers/net/wl/wl.o $(TARGET_MODULES_DIR)
	cp $(LINUX_DIR)/drivers/net/et/et.o $(TARGET_MODULES_DIR)
	cp $(LINUX_DIR)/drivers/net/diag/diag.o $(TARGET_MODULES_DIR)

linux: $(LINUX_DIR)/.modules_done $(TARGET_MODULES_DIR)

linux-source: $(DL_DIR)/$(LINUX_SOURCE)

# This has been renamed so we do _NOT_ by default run this on 'make clean'
linuxclean: clean
	rm -f $(LINUX_KERNEL)
	-$(MAKE) -C $(LINUX_DIR) clean

linux-dirclean:
	rm -f $(BUILD_DIR)/openwrt-kmodules.tar.bz2
	rm -rf $(LINUX_DIR)-$(LINUX_VERSION)
	rm -rf $(LINUX_DIR)
	rm -rf $(BUILD_DIR)/modules
	rm -rf $(BUILD_DIR)/wl
	rm -rf $(BUILD_DIR)/et

