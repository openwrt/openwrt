## tun-module

TUN_MODULE_VERSION := $(SNAPSHOT)
TUN_MODULE_RELEASE := 1

TUN_MODULE_BUILD_DIR := $(BUILD_DIR)/tun-module_$(TUN_MODULE_VERSION)-$(TUN_MODULE_RELEASE)
TUN_MODULE_IPK_DIR := $(OPENWRT_IPK_DIR)/tun-module
TUN_MODULE_IPK := $(TUN_MODULE_BUILD_DIR)_$(ARCH).ipk


$(TUN_MODULE_BUILD_DIR)/CONTROL/control: $(BASE_DIR)/openwrt-kmodules.tar.bz2
	rm -rf $(TUN_MODULE_BUILD_DIR)
	mkdir -p $(TUN_MODULE_BUILD_DIR)/lib/
	bzcat $(BASE_DIR)/openwrt-kmodules.tar.bz2 | tar -C $(TUN_MODULE_BUILD_DIR)/lib/ -xvf - \
	 modules/$(LINUX_VERSION)/kernel/drivers/net/tun.o
	mv -f \
	 $(TUN_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/kernel/drivers/net/tun.o \
	 $(TUN_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/
	rm -rf \
	 $(TUN_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/kernel
	chmod 0755 $(TUN_MODULE_BUILD_DIR)/lib
	chmod 0755 $(TUN_MODULE_BUILD_DIR)/lib/modules
	chmod 0755 $(TUN_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)
	chmod 0755 $(TUN_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/*
	cp -a $(TUN_MODULE_IPK_DIR)/CONTROL $(TUN_MODULE_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(TUN_MODULE_VERSION)-$(TUN_MODULE_RELEASE)/" $(TUN_MODULE_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(TUN_MODULE_BUILD_DIR)/CONTROL/control

	touch $(TUN_MODULE_BUILD_DIR)/CONTROL/control


$(TUN_MODULE_IPK): $(TUN_MODULE_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(TUN_MODULE_BUILD_DIR)


tun-module-ipk: ipkg-utils $(TUN_MODULE_IPK)

tun-module-clean:
	rm -rf $(TUN_MODULE_IPK)
	rm -rf $(TUN_MODULE_BUILD_DIR)

