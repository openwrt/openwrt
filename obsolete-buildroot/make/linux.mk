#############################################################
#
# Linux kernel targets
#
# Note:  If you have any patches to apply, create the directory
# sources/kernel-patches and put your patches in there and number
# them in the order you wish to apply them...  i.e.
#
#   sources/kernel-patches/001-my-special-stuff.bz2
#   sources/kernel-patches/003-gcc-Os.bz2
#   sources/kernel-patches/004_no-warnings.bz2
#   sources/kernel-patches/030-lowlatency-mini.bz2
#   sources/kernel-patches/031-lowlatency-fixes-5.bz2
#   sources/kernel-patches/099-shutup.bz2
#   etc...
#
# these patches will all be applied by the patch-kernel.sh
# script (which will also abort the build if it finds rejects)
#  -Erik
#
#############################################################
ifneq ($(filter $(TARGETS),linux),)

# Version of Linux to download and then apply patches to
DOWNLOAD_LINUX_VERSION=2.4.25
# Version of Linux AFTER patches
LINUX_VERSION=2.4.26-pre5-erik

LINUX_FORMAT=bzImage
#LINUX_FORMAT=images/zImage.prep
LINUX_KARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	)
LINUX_BINLOC=arch/$(LINUX_KARCH)/boot/$(LINUX_FORMAT)

LINUX_DIR=$(BUILD_DIR)/linux-$(LINUX_VERSION)
LINUX_SOURCE=linux-$(DOWNLOAD_LINUX_VERSION).tar.bz2
LINUX_SITE=ftp://ftp.kernel.org/pub/linux/kernel/v2.4
LINUX_KCONFIG=$(SOURCE_DIR)/linux.config
LINUX_KERNEL=$(BUILD_DIR)/buildroot-kernel
# Used by pcmcia-cs and others
LINUX_SOURCE_DIR=$(LINUX_DIR)


$(DL_DIR)/$(LINUX_SOURCE):
	 $(WGET) -P $(DL_DIR) $(LINUX_SITE)/$(LINUX_SOURCE)

$(LINUX_DIR)/.unpacked: $(DL_DIR)/$(LINUX_SOURCE)
	mkdir -p $(LINUX_DIR) $(TOOL_BUILD_DIR)
	rm -rf $(LINUX_DIR)
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | tar -C $(BUILD_DIR) -xvf -
ifneq ($(DOWNLOAD_LINUX_VERSION),$(LINUX_VERSION))
	# Rename the dir from the downloaded version to the AFTER patch version	
	mv -f $(BUILD_DIR)/linux-$(DOWNLOAD_LINUX_VERSION) $(BUILD_DIR)/linux-$(LINUX_VERSION)
endif
	mkdir -p $(SOURCE_DIR)/kernel-patches
	$(SOURCE_DIR)/patch-kernel.sh $(LINUX_DIR) $(SOURCE_DIR)/kernel-patches
	-(cd $(TOOL_BUILD_DIR); ln -sf $(LINUX_DIR) linux)
	touch $(LINUX_DIR)/.unpacked

$(LINUX_KCONFIG):
	@if [ ! -f "$(LINUX_KCONFIG)" ] ; then \
		echo ""; \
		echo "You should create a .config for your kernel"; \
		echo "and install it as $(LINUX_KCONFIG)"; \
		echo ""; \
		sleep 5; \
	fi;

$(LINUX_DIR)/.configured $(BUILD_DIR)/linux/.configured:  $(LINUX_DIR)/.unpacked  $(LINUX_KCONFIG)
	$(SED) "s,^CROSS_COMPILE.*,CROSS_COMPILE=$(KERNEL_CROSS),g;" $(LINUX_DIR)/Makefile
	-cp $(LINUX_KCONFIG) $(LINUX_DIR)/.config
	$(MAKE) -C $(LINUX_DIR) oldconfig include/linux/version.h
	touch $(LINUX_DIR)/.configured

$(LINUX_DIR)/.depend_done:  $(LINUX_DIR)/.configured
	$(MAKE) -C $(LINUX_DIR) dep
	touch $(LINUX_DIR)/.depend_done

$(LINUX_DIR)/$(LINUX_BINLOC): $(LINUX_DIR)/.depend_done
	$(MAKE) -C $(LINUX_DIR) $(LINUX_FORMAT)
	$(MAKE) -C $(LINUX_DIR) modules

$(LINUX_KERNEL): $(LINUX_DIR)/$(LINUX_BINLOC)
	cp -fa $(LINUX_DIR)/$(LINUX_BINLOC) $(LINUX_KERNEL)
	touch -c $(LINUX_KERNEL)

$(TARGET_DIR)/lib/modules/$(LINUX_VERSION)/modules.dep: $(LINUX_KERNEL)
	rm -rf $(TARGET_DIR)/lib/modules
	rm -f $(TARGET_DIR)/sbin/cardmgr
	$(MAKE) -C $(LINUX_DIR) INSTALL_MOD_PATH=$(TARGET_DIR) modules_install
	(cd $(TARGET_DIR)/lib/modules; ln -s $(LINUX_VERSION)/kernel/drivers .)

$(STAGING_DIR)/include/linux/version.h: $(LINUX_DIR)/.configured
	mkdir -p $(STAGING_DIR)/include
	tar -ch -C $(LINUX_DIR)/include -f - linux | tar -xf - -C $(STAGING_DIR)/include/
	tar -ch -C $(LINUX_DIR)/include -f - asm | tar -xf - -C $(STAGING_DIR)/include/

linux: $(STAGING_DIR)/include/linux/version.h $(TARGET_DIR)/lib/modules/$(LINUX_VERSION)/modules.dep

linux-source: $(DL_DIR)/$(LINUX_SOURCE)

# This has been renamed so we do _NOT_ by default run this on 'make clean'
linuxclean: clean
	rm -f $(LINUX_KERNEL)
	-$(MAKE) -C $(LINUX_DIR) clean

linux-dirclean:
	rm -rf $(LINUX_DIR)

endif
