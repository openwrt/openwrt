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

LINUX_FORMAT=zImage
LINUX_KARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	)

LINUX_VERSION=2.4.29
LINUX_CVS_DATE="2005-01-30"
LINUX_CVS_BRANCH=linux_2_4
LINUX_DIR=$(BUILD_DIR)/linux
LINUX_SITE=ftp.linux-mips.org
LINUX_KCONFIG=package/linux/linux.config
LINUX_KERNEL=$(BUILD_DIR)/buildroot-kernel
LINUX_BINLOC=arch/$(LINUX_KARCH)/brcm-boards/bcm947xx/compressed/vmlinuz
# Used by pcmcia-cs and others
LINUX_SOURCE_DIR=$(LINUX_DIR)

LINKSYS_SITE=openwrt.openbsd-geek.de
LINKSYS_TGZ=linksys-3.37.2.tgz

TARGET_MODULES_DIR=$(TARGET_DIR)/lib/modules/$(LINUX_VERSION)

$(DL_DIR)/linux.tar.bz2:
	-mkdir -p $(DL_DIR)
	(cd $(DL_DIR); cvs -d :pserver:cvs:cvs@$(LINUX_SITE):/home/cvs login)
	(cd $(DL_DIR); cvs -z3 -d :pserver:cvs:cvs@$(LINUX_SITE):/home/cvs co -D $(LINUX_CVS_DATE) -r$(LINUX_CVS_BRANCH) linux)
	(cd $(DL_DIR); tar jcvf linux.tar.bz2 linux && rm -rf linux)
	$(WGET) -P $(DL_DIR) $(LINKSYS_SITE)/$(LINKSYS_TGZ)

$(LINUX_DIR)/.unpacked: $(DL_DIR)/linux.tar.bz2
	-mkdir -p $(BUILD_DIR)
	#-(cp -a $(DL_DIR)/linux $(BUILD_DIR)/linux)
	(cd $(BUILD_DIR); tar jxvf $(DL_DIR)/linux.tar.bz2)
	-mkdir -p $(TOOL_BUILD_DIR)
	-(cd $(TOOL_BUILD_DIR); ln -sf $(BUILD_DIR)/linux linux)
	toolchain/patch-kernel.sh $(LINUX_DIR) package/linux/kernel-patches
	-cp $(LINUX_KCONFIG) $(LINUX_DIR)/.config
	# extract linksys binary kernel modules and include/shared files
	-mkdir -p $(BUILD_DIR)/binary
	tar -C $(BUILD_DIR)/binary -xzvf $(DL_DIR)/linksys-3.37.2.tgz
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

linux: $(STAGING_DIR)/include/linux/version.h $(LINUX_DIR)/.modules_done linux-modules-root

linux-modules-root: 
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
