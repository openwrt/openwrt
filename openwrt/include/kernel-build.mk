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

$(LINUX_DIR):
	mkdir -p $@

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

$(LINUX_DIR)/vmlinux: $(STAMP_DIR)/.linux-compile pkg-install ramdisk-config
	$(MAKE) -C $(LINUX_DIR) CROSS_COMPILE="$(KERNEL_CROSS)" ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH)

$(LINUX_KERNEL): $(LINUX_DIR)/vmlinux
	$(TARGET_CROSS)objcopy -O binary -R .reginfo -R .note -R .comment -R .mdebug -S $< $@
	touch -c $(LINUX_KERNEL)

$(LINUX_DIR)/.modules_done:
	rm -rf $(KERNEL_BUILD_DIR)/modules
	$(MAKE) -C "$(LINUX_DIR)" CROSS_COMPILE="$(KERNEL_CROSS)" ARCH=$(LINUX_KARCH) PATH="$(TARGET_PATH)" modules
	$(MAKE) -C "$(LINUX_DIR)" CROSS_COMPILE="$(KERNEL_CROSS)" DEPMOD=true INSTALL_MOD_PATH=$(KERNEL_BUILD_DIR)/modules modules_install
	touch $(LINUX_DIR)/.modules_done

modules: $(LINUX_DIR)/.modules_done
packages: $(TARGETS)

$(STAMP_DIR)/.linux-compile:
	@$(MAKE) modules
	@$(MAKE) packages
	ln -sf $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION) $(BUILD_DIR)/linux
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
	@{ [ "$(INSTALL_TARGETS)" != "" ] && $(IPKG) install $(INSTALL_TARGETS) || true; }

source: $(DL_DIR)/$(LINUX_SOURCE)
prepare: $(LINUX_DIR)/.configured
	@mkdir -p $(STAMP_DIR) $(PACKAGE_DIR)

compile: prepare $(STAMP_DIR)/.linux-compile

install: compile $(LINUX_KERNEL)

mostlyclean: FORCE
	rm -f $(STAMP_DIR)/.linux-compile
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


define AutoLoad
add_module $(1) "$(2)";
endef

define KernelPackage/Defaults
  VERSION:=$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)
  DEPENDS:=
  MAINTAINER:=OpenWrt Developers Team <openwrt-devel@openwrt.org>
  SOURCE:=$(patsubst $(TOPDIR)/%,%,${shell pwd})
  PKGARCH:=$(ARCH)
  PRIORITY:=optional
  KCONFIG:=
  FILES:=
  BUILD:=
  MODULES:=
  TITLE:=
  DESCRIPTION:=
endef

define KernelPackage
  NAME:=$(1)
  $(eval $(call KernelPackage/Defaults))
  $(eval $(call KernelPackage/$(1)))
  $(eval $(call KernelPackage/$(1)/$(KERNEL)))
  
  PKG_$(1) := $(PACKAGE_DIR)/kmod-$(1)_$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)_$(ARCH).ipk
  I_$(1) := $(KMOD_BUILD_DIR)/ipkg/$(1)
  
  IDEPEND_$(1):='kernel ($(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE))' $(DEPENDS)
  
  ifeq ($$(strip $(KCONFIG)),)
    KDEPEND_$(1):=m
  else
    KDEPEND_$(1):=$($(KCONFIG))
  endif
  ifeq ($$(KDEPEND_$(1)),m)
    ifneq ($(CONFIG_PACKAGE_kmod-$(1)),)
      packages: $$(PKG_$(1))
    endif
    ifeq ($(CONFIG_PACKAGE_kmod-$(1)),y)
      install-kmod-$(1): FORCE
		$(IPKG) install $$(PKG_$(1))
      pkg-install: install-kmod-$(1)
    endif
  endif

  $$(PKG_$(1)): $(LINUX_DIR)/.modules_done
	rm -rf $$(I_$(1))
	install -d -m0755 $$(I_$(1))/CONTROL
	echo "Package: kmod-$(1)" > $$(I_$(1))/CONTROL/control
	echo "Version: $(VERSION)" >> $$(I_$(1))/CONTROL/control
	( \
		DEPENDS=; \
		for depend in $$(filter-out @%,$$(IDEPEND_$(1))); do \
			DEPENDS=$$$${DEPENDS:+$$$$DEPENDS, }$$$${depend##+}; \
		done; \
		echo "Depends: $$$$DEPENDS" >> $$(I_$(1))/CONTROL/control; \
	)
	echo "Source: $(SOURCE)" >> $$(I_$(1))/CONTROL/control
	echo "Section: kernel" >> $$(I_$(1))/CONTROL/control
	echo "Priority: $(PRIORITY)" >> $$(I_$(1))/CONTROL/control
	echo "Maintainer: $(MAINTAINER)" >> $$(I_$(1))/CONTROL/control
	echo "Architecture: $(PKGARCH)" >> $$(I_$(1))/CONTROL/control
	echo "Description: $(DESCRIPTION)" | sed -e 's,\\,\n ,g' >> $$(I_$(1))/CONTROL/control
  ifneq ($(strip $(FILES)),)
	mkdir -p $$(I_$(1))/lib/modules/$(LINUX_VERSION)
	$(CP) $(FILES) $$(I_$(1))/lib/modules/$(LINUX_VERSION)/
  endif
  ifneq ($(MODULES),)
	export modules=; \
	add_module() { \
		mkdir -p $$(I_$(1))/etc/modules.d; \
		echo "$$$$2" > $$(I_$(1))/etc/modules.d/$$$$1-$(1); \
		modules="$$$${modules:+$$$$modules }$$$$1-$(1)"; \
	}; \
	$(MODULES) \
	mkdir -p $$(I_$(1))/etc/modules.d; \
	echo "#!/bin/sh" >> $$(I_$(1))/CONTROL/postinst; \
	echo "[ -z \"\$$$$IPKG_INSTROOT\" ] || exit" >> $$(I_$(1))/CONTROL/postinst; \
	echo ". /etc/functions.sh" >> $$(I_$(1))/CONTROL/postinst; \
	echo "load_modules $$$$modules" >> $$(I_$(1))/CONTROL/postinst; \
	chmod 0755 $$(I_$(1))/CONTROL/postinst;
  endif
	$(IPKG_BUILD) $$(I_$(1)) $(PACKAGE_DIR)
endef

-include $(INCLUDE_DIR)/modules.mk

