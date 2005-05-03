#############################################################
# $Id$
#
# Linux kernel target for the OpenWRT project
#
# patches are sorted by numbers
# 000	patch between linux-2.4.xx and linux-mips-cvs
# 0xx	linksys patches
# 1xx	OpenWRT patches (diag,compressed,..)
# 2xx	fixes for wl driver integration
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

# proprietary driver extracted from linksys firmware GPL sourcetree WRT54GS_3_37_2_1109_US 
LINUX_BINARY_WL_DRIVER=kernel-binary-wl-0.2.tar.gz
LINUX_BINARY_WL_MD5SUM=ab2a6d39ccb550e494bbeccf1b0e228f
LINUX_ET_DRIVER=kernel-source-et-0.6.tar.gz
LINUX_ET_MD5SUM=b7883403d3c1ce7b73c362db621c6f00

TARGET_MODULES_DIR=$(TARGET_DIR)/lib/modules/$(LINUX_VERSION)

$(DL_DIR)/$(LINUX_BINARY_WL_DRIVER):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(LINUX_BINARY_WL_DRIVER) $(LINUX_BINARY_WL_MD5SUM) $(LINUX_BINARY_DRIVER_SITE)

$(DL_DIR)/$(LINUX_ET_DRIVER):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(LINUX_ET_DRIVER) $(LINUX_ET_MD5SUM) $(LINUX_BINARY_DRIVER_SITE)

$(LINUX_DIR)/.unpacked: $(DL_DIR)/$(LINUX_SOURCE) $(DL_DIR)/$(LINUX_BINARY_WL_DRIVER) $(DL_DIR)/$(LINUX_ET_DRIVER)
	-mkdir -p $(BUILD_DIR)
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	ln -sf $(LINUX_DIR)-$(LINUX_VERSION) $(LINUX_DIR)
	zcat $(DL_DIR)/$(LINUX_BINARY_WL_DRIVER) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	zcat $(DL_DIR)/$(LINUX_ET_DRIVER) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(LINUX_DIR)/.unpacked

$(LINUX_DIR)/.patched: $(LINUX_DIR)/.unpacked
	$(PATCH) $(LINUX_DIR) $(LINUX_PATCHES)
	# copy kernel source which is maintained in openwrt via cvs
	cp -a $(LINUX_KERNEL_SOURCE)/* $(LINUX_DIR)
	# copy binary wlan driver
	cp -a $(BUILD_DIR)/wl/*.o $(LINUX_DIR)/drivers/net/wl
	# copy proprietary et source
	cp -a $(BUILD_DIR)/et/* $(LINUX_DIR)/drivers/net/et
	cp -a $(BUILD_DIR)/et/*.h $(LINUX_DIR)/include/
	$(SED) 's/@expr length/@-expr length/' $(LINUX_DIR)/Makefile 
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
	$(MAKE) -C $(LINUX_DIR) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) CFLAGS_KERNEL="-fno-delayed-branch " $(LINUX_FORMAT)

$(LINUX_KERNEL): $(LINUX_DIR)/$(LINUX_BINLOC)
	cp -fa $< $@ 
	touch -c $(LINUX_KERNEL)
	
$(LINUX_IMAGE): $(LINUX_KERNEL)
	cat $^ | $(BUILD_DIR)/lzma/lzma e -si -so -eos > $@ || (rm -f $@ && false)

$(LINUX_DIR)/.modules_done: $(LINUX_KERNEL) $(LINUX_IMAGE)
	rm -rf $(BUILD_DIR)/modules
	$(MAKE) -C $(LINUX_DIR) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) CFLAGS_KERNEL="-fno-delayed-branch " modules
	$(MAKE) -C $(LINUX_DIR) DEPMOD=true INSTALL_MOD_PATH=$(BUILD_DIR)/modules modules_install
	touch $(LINUX_DIR)/.modules_done

$(STAGING_DIR)/include/linux/version.h: $(LINUX_DIR)/.configured
	mkdir -p $(STAGING_DIR)/include
	tar -ch -C $(LINUX_DIR)/include -f - linux | tar -xf - -C $(STAGING_DIR)/include/
	tar -ch -C $(LINUX_DIR)/include -f - asm | tar -xf - -C $(STAGING_DIR)/include/

$(TARGET_MODULES_DIR): 
	-mkdir -p $(TARGET_MODULES_DIR)

linux: $(LINUX_DIR)/.modules_done $(TARGET_MODULES_DIR)

linux-source: $(DL_DIR)/$(LINUX_SOURCE)

# This has been renamed so we do _NOT_ by default run this on 'make clean'
linuxclean: clean
	rm -f $(LINUX_KERNEL) $(LINUX_IMAGE)
	-$(MAKE) -C $(LINUX_DIR) clean

linux-dirclean:
	rm -f $(BUILD_DIR)/openwrt-kmodules.tar.bz2
	rm -rf $(LINUX_DIR)-$(LINUX_VERSION)
	rm -rf $(LINUX_DIR)
	rm -rf $(BUILD_DIR)/modules
	rm -rf $(BUILD_DIR)/wl
	rm -rf $(BUILD_DIR)/et

