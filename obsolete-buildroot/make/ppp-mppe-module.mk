## ppp-mppe-module

PPP_MPPE_MODULE_VERSION := $(SNAPSHOT)
PPP_MPPE_MODULE_RELEASE := 1

PPP_MPPE_MODULE_BUILD_DIR := $(BUILD_DIR)/ppp-mppe-module_$(PPP_MPPE_MODULE_VERSION)-$(PPP_MPPE_MODULE_RELEASE)
PPP_MPPE_MODULE_IPK_DIR := $(OPENWRT_IPK_DIR)/ppp-mppe-module
PPP_MPPE_MODULE_IPK := $(PPP_MPPE_MODULE_BUILD_DIR)_$(ARCH).ipk


$(PPP_MPPE_MODULE_BUILD_DIR)/CONTROL/control: $(BASE_DIR)/openwrt-kmodules.tar.bz2
	rm -rf $(PPP_MPPE_MODULE_BUILD_DIR)
	mkdir -p $(PPP_MPPE_MODULE_BUILD_DIR)/lib/
	bzcat $(BASE_DIR)/openwrt-kmodules.tar.bz2 | tar -C $(PPP_MPPE_MODULE_BUILD_DIR)/lib/ -xvf - \
	 modules/$(LINUX_VERSION)/kernel/drivers/net/ppp_mppe_mppc.o
	mv -f \
	 $(PPP_MPPE_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/kernel/drivers/net/ppp_mppe_mppc.o \
	 $(PPP_MPPE_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/
	rm -rf \
	 $(PPP_MPPE_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/kernel
	chmod 0755 $(PPP_MPPE_MODULE_BUILD_DIR)/lib
	chmod 0755 $(PPP_MPPE_MODULE_BUILD_DIR)/lib/modules
	chmod 0755 $(PPP_MPPE_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)
	chmod 0755 $(PPP_MPPE_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/*
	cp -a $(PPP_MPPE_MODULE_IPK_DIR)/CONTROL $(PPP_MPPE_MODULE_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(PPP_MPPE_MODULE_VERSION)-$(PPP_MPPE_MODULE_RELEASE)/" $(PPP_MPPE_MODULE_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(PPP_MPPE_MODULE_BUILD_DIR)/CONTROL/control

	touch $(PPP_MPPE_MODULE_BUILD_DIR)/CONTROL/control


$(PPP_MPPE_MODULE_IPK): $(PPP_MPPE_MODULE_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(PPP_MPPE_MODULE_BUILD_DIR)


ppp-mppe-module-ipk: ipkg-utils $(PPP_MPPE_MODULE_IPK)

ppp-mppe-module-clean:
	rm -rf $(PPP_MPPE_MODULE_IPK)
	rm -rf $(PPP_MPPE_MODULE_BUILD_DIR)

