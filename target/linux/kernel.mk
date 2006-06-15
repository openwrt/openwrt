LINUX_SOURCE:=$(LINUX_NAME).tar.bz2
LINUX_SITE=http://www.us.kernel.org/pub/linux/kernel/v$(KERNEL) \
           http://www.us.kernel.org/pub/linux/kernel/v$(KERNEL) \
           http://www.kernel.org/pub/linux/kernel/v$(KERNEL) \
           http://www.de.kernel.org/pub/linux/kernel/v$(KERNEL)

KERNEL_IDIR:=$(LINUX_BUILD_DIR)/kernel-ipkg

$(TARGETS): $(PACKAGE_DIR)

$(LINUX_DIR):
	mkdir -p $@

$(PACKAGE_DIR):
	mkdir -p $@

$(DL_DIR)/$(LINUX_SOURCE):
	-mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(LINUX_SOURCE) $(LINUX_KERNEL_MD5SUM) $(LINUX_SITE)

$(LINUX_DIR)/.unpacked: $(DL_DIR)/$(LINUX_SOURCE)
	-mkdir -p $(LINUX_BUILD_DIR)
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | tar -C $(LINUX_BUILD_DIR) $(TAR_OPTIONS) -
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

$(LINUX_DIR)/vmlinux: $(STAMP_DIR)/.linux-compile pkg-install ramdisk-config
	$(MAKE) -C $(LINUX_DIR) CROSS_COMPILE="$(KERNEL_CROSS)" ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH)

$(LINUX_KERNEL): $(LINUX_DIR)/vmlinux
	$(TARGET_CROSS)objcopy -O binary -R .reginfo -R .note -R .comment -R .mdebug -S $< $@
	touch -c $(LINUX_KERNEL)

$(LINUX_DIR)/.modules_done:
	rm -rf $(LINUX_BUILD_DIR)/modules
	$(MAKE) -C "$(LINUX_DIR)" CROSS_COMPILE="$(KERNEL_CROSS)" ARCH=$(LINUX_KARCH) PATH="$(TARGET_PATH)" modules
	$(MAKE) -C "$(LINUX_DIR)" CROSS_COMPILE="$(KERNEL_CROSS)" DEPMOD=true INSTALL_MOD_PATH=$(LINUX_BUILD_DIR)/modules modules_install
	touch $(LINUX_DIR)/.modules_done

$(STAMP_DIR)/.linux-compile:
	@$(MAKE) $(LINUX_DIR)/.modules_done $(TARGETS) $(KERNEL_IPKG)
	ln -sf $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION) $(BUILD_DIR)/linux
	touch $@

$(KERNEL_IPKG):
	rm -rf $(KERNEL_IDIR)
	mkdir -p $(KERNEL_IDIR)/etc
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(KERNEL_IDIR) ../control/kernel.control $(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE) $(ARCH)
	if [ -f ./config/$(BOARD).modules ]; then \
		cp ./config/$(BOARD).modules $(KERNEL_IDIR)/etc/modules; \
	fi
	$(IPKG_BUILD) $(KERNEL_IDIR) $(LINUX_BUILD_DIR)

$(BUILD_DIR)/kernel.mk: $(LINUX_DIR) FORCE
	echo "BOARD:=$(BOARD)" > $@
	echo "LINUX_NAME:=$(LINUX_NAME)" >> $@
	echo "LINUX_VERSION:=$(LINUX_VERSION)" >> $@
	echo "LINUX_RELEASE:=$(LINUX_RELEASE)" >> $@

pkg-install: FORCE
	@{ [ "$(INSTALL_TARGETS)" != "" ] && $(IPKG) install $(INSTALL_TARGETS) || true; }

source: $(DL_DIR)/$(LINUX_SOURCE)
prepare: $(BUILD_DIR)/kernel.mk
	@mkdir -p $(STAMP_DIR) $(PACKAGE_DIR)
	@$(MAKE) $(LINUX_DIR)/.configured

compile: prepare $(STAMP_DIR)/.linux-compile

install: compile $(LINUX_KERNEL)

mostlyclean: FORCE
	rm -f $(STAMP_DIR)/.linux-compile
	rm -f $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION)/.modules_done
	rm -f $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION)/.drivers-unpacked
	$(MAKE) -C $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION) clean
	rm -f $(LINUX_KERNEL)

rebuild: FORCE
	-$(MAKE) mostlyclean
	if [ -f $(LINUX_KERNEL) ]; then \
		$(MAKE) clean; \
	fi
	$(MAKE) compile $(MAKE_TRACE)

clean: FORCE
	rm -f $(STAMP_DIR)/.linux-compile
	rm -rf $(LINUX_BUILD_DIR)
	rm -f $(TARGETS)
