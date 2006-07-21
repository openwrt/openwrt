# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
-include $(INCLUDE_DIR)/modules-$(KERNEL).mk

LINUX_SOURCE:=linux-$(LINUX_VERSION).tar.bz2
LINUX_SITE=http://www.us.kernel.org/pub/linux/kernel/v$(KERNEL) \
           http://www.us.kernel.org/pub/linux/kernel/v$(KERNEL) \
           http://www.kernel.org/pub/linux/kernel/v$(KERNEL) \
           http://www.de.kernel.org/pub/linux/kernel/v$(KERNEL)

KERNEL_IDIR:=$(KERNEL_BUILD_DIR)/kernel-ipkg
KERNEL_IPKG:=$(KERNEL_BUILD_DIR)/kernel_$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)_$(ARCH).ipk
TARGETS += $(KERNEL_IPKG)
INSTALL_TARGETS += $(KERNEL_IPKG)

$(TARGETS): $(PACKAGE_DIR)

$(PACKAGE_DIR):
	mkdir -p $@

$(DL_DIR)/$(LINUX_SOURCE):
	-mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(LINUX_SOURCE) $(LINUX_KERNEL_MD5SUM) $(LINUX_SITE)

$(LINUX_DIR)/.unpacked: $(DL_DIR)/$(LINUX_SOURCE)
	-mkdir -p $(KERNEL_BUILD_DIR)
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | tar -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

ifeq ($(KERNEL),2.4)
$(LINUX_DIR)/.configured: $(LINUX_DIR)/.patched
	$(SED) "s,^CROSS_COMPILE.*,CROSS_COMPILE=$(KERNEL_CROSS),g;" \
	  $(LINUX_DIR)/Makefile  \
	  $(LINUX_DIR)/arch/*/Makefile
	$(SED) "s,\-mcpu=,\-mtune=,g;" $(LINUX_DIR)/arch/mips/Makefile
	$(MAKE) -C $(LINUX_DIR) ARCH=$(LINUX_KARCH) oldconfig include/linux/compile.h include/linux/version.h
	touch $@

$(LINUX_DIR)/.depend_done: $(LINUX_DIR)/.configured
	$(MAKE) -C $(LINUX_DIR) CROSS_COMPILE="$(KERNEL_CROSS)" ARCH=$(LINUX_KARCH) dep
	touch $@

$(LINUX_DIR)/vmlinux: $(LINUX_DIR)/.depend_done
else
$(LINUX_DIR)/.configured: $(LINUX_DIR)/.patched
	$(MAKE) -C $(LINUX_DIR) CROSS_COMPILE="$(KERNEL_CROSS)" ARCH=$(LINUX_KARCH) oldconfig prepare scripts
	touch $@
endif

ramdisk-config: $(LINUX_DIR)/.configured FORCE
	mv $(LINUX_DIR)/.config $(LINUX_DIR)/.config.old
	grep -v INITRAMFS $(LINUX_DIR)/.config.old > $(LINUX_DIR)/.config
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	echo 'CONFIG_INITRAMFS_SOURCE="../../root"' >> $(LINUX_DIR)/.config
	echo 'CONFIG_INITRAMFS_ROOT_UID=0' >> $(LINUX_DIR)/.config
	echo 'CONFIG_INITRAMFS_ROOT_GID=0' >> $(LINUX_DIR)/.config
	mkdir -p $(BUILD_DIR)/root/etc/init.d
	$(CP) ../generic-2.6/files/init $(BUILD_DIR)/root/
else
	rm -f $(BUILD_DIR)/root/init $(BUILD_DIR)/root/etc/init.d/S00initramfs
	echo 'CONFIG_INITRAMFS_SOURCE=""' >> $(LINUX_DIR)/.config
endif

$(LINUX_DIR)/vmlinux: $(LINUX_DIR)/.linux-compile pkg-install ramdisk-config
	$(MAKE) -C $(LINUX_DIR) CROSS_COMPILE="$(KERNEL_CROSS)" ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH)

$(LINUX_KERNEL): $(LINUX_DIR)/vmlinux
	$(KERNEL_CROSS)objcopy -O binary -R .reginfo -R .note -R .comment -R .mdebug -S $< $@
	touch -c $(LINUX_KERNEL)

$(LINUX_DIR)/.modules_done:
	rm -rf $(KERNEL_BUILD_DIR)/modules
	$(MAKE) -C "$(LINUX_DIR)" CROSS_COMPILE="$(KERNEL_CROSS)" ARCH=$(LINUX_KARCH) PATH="$(TARGET_PATH)" modules
	$(MAKE) -C "$(LINUX_DIR)" CROSS_COMPILE="$(KERNEL_CROSS)" ARCH=$(LINUX_KARCH) DEPMOD=true INSTALL_MOD_PATH=$(KERNEL_BUILD_DIR)/modules modules_install
	touch $(LINUX_DIR)/.modules_done

modules: $(LINUX_DIR)/.modules_done
packages: $(TARGETS)

$(LINUX_DIR)/.linux-compile:
	ln -sf $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION) $(BUILD_DIR)/linux
	@$(MAKE) modules
	@$(MAKE) packages
	touch $@

$(KERNEL_IPKG):
	rm -rf $(KERNEL_IDIR)
	mkdir -p $(KERNEL_IDIR)/etc
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(KERNEL_IDIR) ../control/kernel.control $(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE) $(ARCH)
	if [ -f ./config/$(BOARD).modules ]; then \
		cp ./config/$(BOARD).modules $(KERNEL_IDIR)/etc/modules; \
	fi
	$(IPKG_BUILD) $(KERNEL_IDIR) $(KERNEL_BUILD_DIR)

$(TOPDIR)/.kernel.mk:
	echo "BOARD:=$(BOARD)" > $@
	echo "LINUX_VERSION:=$(LINUX_VERSION)" >> $@
	echo "LINUX_RELEASE:=$(LINUX_RELEASE)" >> $@

pkg-install: FORCE
	@for pkg in $(INSTALL_TARGETS); do \
		$(IPKG) install $$pkg || echo; \
	done

download: $(DL_DIR)/$(LINUX_SOURCE)
prepare: $(LINUX_DIR)/.configured
	@mkdir -p $(LINUX_DIR) $(PACKAGE_DIR)

compile: prepare $(LINUX_DIR)/.linux-compile

install: compile $(LINUX_KERNEL)

mostlyclean: FORCE
	rm -f $(LINUX_DIR)/.linux-compile
	rm -f $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)/.modules_done
	rm -f $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)/.drivers-unpacked
	$(MAKE) -C $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION) clean
	rm -f $(LINUX_KERNEL)

rebuild: FORCE
	-$(MAKE) mostlyclean
	if [ -f $(LINUX_KERNEL) ]; then \
		$(MAKE) clean; \
	fi
	$(MAKE) compile $(MAKE_TRACE)

clean: FORCE
	rm -f $(STAMP_DIR)/.linux-compile
	rm -rf $(KERNEL_BUILD_DIR)
	rm -f $(TARGETS)

