#############################################################
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
ifneq ($(filter $(TARGETS),linux),)

LINUX_FORMAT=zImage
LINUX_KARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	)

LINUX_VERSION=2.4.29
LINUX_DIR=$(BUILD_DIR)/linux-$(LINUX_VERSION)
LINUX_SITE=http://www.kernel.org/pub/linux/kernel/v2.4
LINUX_SOURCE=linux-$(LINUX_VERSION).tar.bz2
LINUX_KCONFIG=package/linux/linux.config
LINUX_KERNEL=$(BUILD_DIR)/buildroot-kernel
LINUX_BINLOC=arch/$(LINUX_KARCH)/brcm-boards/bcm947xx/compressed/vmlinuz
# Used by pcmcia-cs and others
LINUX_SOURCE_DIR=$(LINUX_DIR)

# kernel stuff extracted from linksys firmware GPL sourcetree
# WRT54GS_3_37_2_1109_US (shared,include,wl,et)
LINKSYS_KERNEL_TGZ=linksys-kernel.tar.gz

TARGET_MODULES_DIR=$(TARGET_DIR)/lib/modules/$(LINUX_VERSION)

$(DL_DIR)/$(LINUX_SOURCE):
	-mkdir -p $(DL_DIR)
	$(WGET) -P $(DL_DIR) $(LINUX_SITE)/$(LINUX_SOURCE)

$(DL_DIR)/$(LINKSYS_KERNEL_TGZ):
	$(WGET) -P $(DL_DIR) $(LINKSYS_TGZ_SITE)/$(LINKSYS_KERNEL_TGZ)

$(LINUX_DIR)/.unpacked: $(DL_DIR)/$(LINUX_SOURCE) $(DL_DIR)/$(LINKSYS_KERNEL_TGZ)
	-mkdir -p $(BUILD_DIR)
	(cd $(BUILD_DIR); tar jxvf $(DL_DIR)/$(LINUX_SOURCE))
	-mkdir -p $(TOOL_BUILD_DIR)
	-(cd $(TOOL_BUILD_DIR); ln -sf $(LINUX_DIR) linux)
	toolchain/patch-kernel.sh $(LINUX_DIR) package/linux/kernel-patches
	-cp $(LINUX_KCONFIG) $(LINUX_DIR)/.config
	# extract linksys binary kernel stuff and include/shared files
	zcat $(DL_DIR)/$(LINKSYS_KERNEL_TGZ) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(LINUX_DIR)/.unpacked

$(LINUX_KCONFIG):
	@if [ ! -f "$(LINUX_KCONFIG)" ] ; then \
		echo ""; \
		echo "You should create a .config for your kernel"; \
		echo "and install it as $(LINUX_KCONFIG)"; \
		echo ""; \
		sleep 5; \
	fi;

$(LINUX_DIR)/.configured:  $(LINUX_DIR)/.unpacked  $(LINUX_KCONFIG)
	$(SED) "s,^CROSS_COMPILE.*,CROSS_COMPILE=$(KERNEL_CROSS),g;" $(LINUX_DIR)/Makefile
	$(SED) "s,^CROSS_COMPILE.*,CROSS_COMPILE=$(KERNEL_CROSS),g;" $(LINUX_DIR)/arch/mips/Makefile
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
	tar -C $(BUILD_DIR)/modules/lib -cjf openwrt-kmodules.tar.bz2 modules
	touch $(LINUX_DIR)/.modules_done


$(STAGING_DIR)/include/linux/version.h: $(LINUX_DIR)/.configured
	mkdir -p $(STAGING_DIR)/include
	tar -ch -C $(LINUX_DIR)/include -f - linux | tar -xf - -C $(STAGING_DIR)/include/
	tar -ch -C $(LINUX_DIR)/include -f - asm | tar -xf - -C $(STAGING_DIR)/include/

linux: $(LINUX_DIR)/.modules_done $(TARGET_MODULES_DIR)

$(TARGET_MODULES_DIR): 
	-mkdir -p $(TARGET_MODULES_DIR)
	cp $(LINUX_DIR)/drivers/net/wl/wl.o $(TARGET_MODULES_DIR)
	cp $(LINUX_DIR)/drivers/net/et/et.o $(TARGET_MODULES_DIR)
	cp $(LINUX_DIR)/drivers/net/diag/diag.o $(TARGET_MODULES_DIR)

linux-source: $(DL_DIR)/$(LINUX_SOURCE)

# This has been renamed so we do _NOT_ by default run this on 'make clean'
linuxclean: clean
	rm -f $(LINUX_KERNEL)
	-$(MAKE) -C $(LINUX_DIR) clean

linux-dirclean:
	rm -rf $(LINUX_DIR)

linux-headers: $(LINUX_DIR)/.configured 

endif
