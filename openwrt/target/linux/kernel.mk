LINUX_SOURCE:=linux-$(LINUX_VERSION).tar.bz2
LINUX_SITE=http://www.us.kernel.org/pub/linux/kernel/v$(KERNEL) \
           http://www.us.kernel.org/pub/linux/kernel/v$(KERNEL) \
           http://www.kernel.org/pub/linux/kernel/v$(KERNEL) \
           http://www.de.kernel.org/pub/linux/kernel/v$(KERNEL)

KERNEL_IDIR:=$(LINUX_BUILD_DIR)/kernel-ipkg

$(TARGETS): $(PACKAGE_DIR)

$(PACKAGE_DIR):
	mkdir -p $(PACKAGE_DIR)
	
$(DL_DIR)/$(LINUX_SOURCE):
	-mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(LINUX_SOURCE) $(LINUX_KERNEL_MD5SUM) $(LINUX_SITE) $(MAKE_TRACE)

$(LINUX_DIR)/.unpacked: $(DL_DIR)/$(LINUX_SOURCE)
	-mkdir -p $(LINUX_BUILD_DIR)
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | tar -C $(LINUX_BUILD_DIR) $(TAR_OPTIONS) -
	rm -f $(BUILD_DIR)/linux
	ln -s $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION) $(BUILD_DIR)/linux
	touch $@

ifeq ($(KERNEL),2.4)
$(LINUX_DIR)/.configured: $(LINUX_DIR)/.patched
	$(SED) "s,^CROSS_COMPILE.*,CROSS_COMPILE=$(KERNEL_CROSS),g;" \
	  $(LINUX_DIR)/Makefile  \
	  $(LINUX_DIR)/arch/*/Makefile
	$(SED) "s,\-mcpu=,\-mtune=,g;" $(LINUX_DIR)/arch/mips/Makefile
	$(MAKE) -C $(LINUX_DIR) ARCH=$(LINUX_KARCH) oldconfig include/linux/compile.h include/linux/version.h $(MAKE_TRACE)
	touch $@

$(LINUX_DIR)/.depend_done: $(LINUX_DIR)/.configured
	$(MAKE) -C $(LINUX_DIR) CROSS_COMPILE="$(KERNEL_CROSS)" ARCH=$(LINUX_KARCH) dep $(MAKE_TRACE)
	touch $@

$(LINUX_DIR)/vmlinux: $(LINUX_DIR)/.depend_done
else
$(LINUX_DIR)/.configured: $(LINUX_DIR)/.patched
	$(MAKE) -C $(LINUX_DIR) CROSS_COMPILE="$(KERNEL_CROSS)" ARCH=$(LINUX_KARCH) oldconfig prepare scripts $(MAKE_TRACE)
	touch $@
endif

$(LINUX_DIR)/vmlinux: $(STAMP_DIR)/.linux-compile
	$(MAKE) -C $(LINUX_DIR) CROSS_COMPILE="$(KERNEL_CROSS)" ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) $(MAKE_TRACE)

$(LINUX_KERNEL): $(LINUX_DIR)/vmlinux
	$(TARGET_CROSS)objcopy -O binary -R .reginfo -R .note -R .comment -R .mdebug -S $< $@ $(MAKE_TRACE)
	touch -c $(LINUX_KERNEL)

$(LINUX_DIR)/.modules_done:
	rm -rf $(LINUX_BUILD_DIR)/modules
	$(MAKE) -C "$(LINUX_DIR)" CROSS_COMPILE="$(KERNEL_CROSS)" ARCH=$(LINUX_KARCH) PATH="$(TARGET_PATH)" modules $(MAKE_TRACE)
	$(MAKE) -C "$(LINUX_DIR)" CROSS_COMPILE="$(KERNEL_CROSS)" DEPMOD=true INSTALL_MOD_PATH=$(LINUX_BUILD_DIR)/modules modules_install $(MAKE_TRACE)
	touch $(LINUX_DIR)/.modules_done

$(STAMP_DIR)/.linux-compile:
	@$(MAKE) $(LINUX_DIR)/.modules_done $(TARGETS) $(KERNEL_IPKG) $(MAKE_TRACE)
	ln -sf $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION) $(BUILD_DIR)/linux $(MAKE_TRACE)
	@$(TRACE) target/linux/package-compile
	$(MAKE) -C $(TOPDIR)/target/linux/package \
		$(KPKG_MAKEOPTS) \
		compile
	touch $@

.PHONY: pkg-install
pkg-install:
	@mkdir -p $(TARGET_MODULES_DIR)
	@rm -rf $(LINUX_BUILD_DIR)/root*
	@cp -fpR $(BUILD_DIR)/root $(LINUX_BUILD_DIR)/
	echo -e 'dest root /\noption offline_root $(LINUX_BUILD_DIR)/root' > $(LINUX_BUILD_DIR)/ipkg.conf
	$(MAKE) -C $(TOPDIR)/target/linux/package \
		$(KPKG_MAKEOPTS) \
		install
	@{ [ "$(INSTALL_TARGETS)" != "" ] && $(IPKG_KERNEL) install $(INSTALL_TARGETS) || true; } $(MAKE_TRACE) 

$(KERNEL_IPKG):
	rm -rf $(KERNEL_IDIR)
	mkdir -p $(KERNEL_IDIR)/etc
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(KERNEL_IDIR) ../control/kernel.control $(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE) $(ARCH)
	if [ -f ./config/$(BOARD).modules ]; then \
		cp ./config/$(BOARD).modules $(KERNEL_IDIR)/etc/modules; \
	fi
	$(IPKG_BUILD) $(KERNEL_IDIR) $(LINUX_BUILD_DIR) $(MAKE_TRACE)

source: $(DL_DIR)/$(LINUX_SOURCE)
prepare: 
	@mkdir -p $(STAMP_DIR) $(PACKAGE_DIR)
	@$(MAKE) $(LINUX_DIR)/.configured $(MAKE_TRACE)

compile: prepare $(STAMP_DIR)/.linux-compile

install: compile
	@$(TRACE) target/linux/package-install
	$(MAKE) $(KPKG_MAKEOPTS) pkg-install $(MAKE_TRACE)
	$(MAKE) $(KPKG_MAKEOPTS) $(LINUX_KERNEL) $(MAKE_TRACE)

mostlyclean:
	rm -f $(STAMP_DIR)/.linux-compile
	rm -f $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION)/.modules_done
	rm -f $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION)/.drivers-unpacked
	$(MAKE) -C $(LINUX_BUILD_DIR)/linux-$(LINUX_VERSION) clean $(MAKE_TRACE)
	rm -f $(LINUX_KERNEL)

rebuild:
	-$(MAKE) mostlyclean
	if [ -f $(LINUX_KERNEL) ]; then \
		$(MAKE) clean $(MAKE_TRACE); \
	fi
	$(MAKE) compile $(MAKE_TRACE)

clean:
	rm -f $(STAMP_DIR)/.linux-compile
	rm -rf $(LINUX_BUILD_DIR)
	rm -f $(TARGETS)

package/%:
	$(MAKE) -C $(TOPDIR)/target/linux/package \
		$(KPKG_MAKEOPTS) \
		$(patsubst package/%,%,$@)
