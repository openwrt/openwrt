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

LINUX_FORMAT=zImage
LINUX_KARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	)

LINUX_SITE=http://www.kernel.org/pub/linux/kernel/v2.4
LINUX_SOURCE=linux-$(LINUX_VERSION).tar.bz2
LINUX_KCONFIG=./linux.config
LINUX_PATCHES=./kernel-patches
LINUX_BINLOC=arch/$(LINUX_KARCH)/brcm-boards/bcm947xx/compressed/vmlinuz
# Used by pcmcia-cs and others
LINUX_SOURCE_DIR=$(LINUX_DIR)-$(LINUX_VERSION)

# kernel stuff extracted from linksys firmware GPL sourcetree
# WRT54GS_3_37_2_1109_US (shared,include,wl,et)
LINKSYS_KERNEL_SITE=http://openwrt.openbsd-geek.de
LINKSYS_KERNEL_TGZ=linksys-kernel.tar.gz

TARGET_MODULES_DIR=$(TARGET_DIR)/lib/modules/$(LINUX_VERSION)

$(DL_DIR)/$(LINKSYS_KERNEL_TGZ):
	$(WGET) -P $(DL_DIR) $(LINKSYS_KERNEL_SITE)/$(LINKSYS_KERNEL_TGZ)

$(LINUX_DIR)/.unpacked: $(DL_DIR)/$(LINUX_SOURCE) $(DL_DIR)/$(LINKSYS_KERNEL_TGZ)
	-mkdir -p $(BUILD_DIR)
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	# extract linksys binary kernel stuff and include/shared files
	zcat $(DL_DIR)/$(LINKSYS_KERNEL_TGZ) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	ln -sf $(LINUX_DIR)-$(LINUX_VERSION) $(LINUX_DIR)
	touch $(LINUX_DIR)/.unpacked

$(LINUX_DIR)/.patched: $(LINUX_DIR)/.unpacked
	$(PATCH) $(LINUX_DIR) $(LINUX_PATCHES)
	touch $(LINUX_DIR)/.patched

$(LINUX_DIR)/.configured: $(LINUX_DIR)/.patched
	-cp $(LINUX_KCONFIG) $(LINUX_DIR)/.config
#ifeq ($(BR2_TARGET_ROOTFS_SQUASHFS_LZMA),y)
#	$(SED) "s,rootfstype=jffs2,rootfstype=squashfs," $(LINUX_DIR)/.config
#endif
#ifeq ($(BR2_TARGET_ROOTFS_SQUASHFS),y)
#	$(SED) "s,rootfstype=jffs2,rootfstype=squashfs," $(LINUX_DIR)/.config
#endif
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
	rm -rf $(BUILD_DIR)/linksys-kernel

