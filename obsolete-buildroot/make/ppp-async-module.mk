## ppp-async-module

PPP_ASYNC_MODULE_VERSION := $(SNAPSHOT)
PPP_ASYNC_MODULE_RELEASE := 1

PPP_ASYNC_MODULE_BUILD_DIR := $(BUILD_DIR)/ppp-async-module_$(PPP_ASYNC_MODULE_VERSION)-$(PPP_ASYNC_MODULE_RELEASE)
PPP_ASYNC_MODULE_IPK_DIR := $(OPENWRT_IPK_DIR)/ppp-async-module
PPP_ASYNC_MODULE_IPK := $(PPP_ASYNC_MODULE_BUILD_DIR)_$(ARCH).ipk


$(PPP_ASYNC_MODULE_BUILD_DIR)/CONTROL/control: $(BASE_DIR)/openwrt-kmodules.tar.bz2
	rm -rf $(PPP_ASYNC_MODULE_BUILD_DIR)
	mkdir -p $(PPP_ASYNC_MODULE_BUILD_DIR)/lib/
	bzcat $(BASE_DIR)/openwrt-kmodules.tar.bz2 | tar -C $(PPP_ASYNC_MODULE_BUILD_DIR)/lib/ -xvf - \
	 modules/$(LINUX_VERSION)/kernel/drivers/net/ppp_async.o
	mv -f \
	 $(PPP_ASYNC_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/kernel/drivers/net/ppp_async.o \
	 $(PPP_ASYNC_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/
	rm -rf \
	 $(PPP_ASYNC_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/kernel
	chmod 0755 $(PPP_ASYNC_MODULE_BUILD_DIR)/lib
	chmod 0755 $(PPP_ASYNC_MODULE_BUILD_DIR)/lib/modules
	chmod 0755 $(PPP_ASYNC_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)
	chmod 0755 $(PPP_ASYNC_MODULE_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/*
	cp -a $(PPP_ASYNC_MODULE_IPK_DIR)/CONTROL $(PPP_ASYNC_MODULE_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(PPP_ASYNC_MODULE_VERSION)-$(PPP_ASYNC_MODULE_RELEASE)/" $(PPP_ASYNC_MODULE_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(PPP_ASYNC_MODULE_BUILD_DIR)/CONTROL/control

	touch $(PPP_ASYNC_MODULE_BUILD_DIR)/CONTROL/control


$(PPP_ASYNC_MODULE_IPK): $(PPP_ASYNC_MODULE_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(PPP_ASYNC_MODULE_BUILD_DIR)


ppp-async-module-ipk: ipkg-utils $(PPP_ASYNC_MODULE_IPK)

ppp-async-module-clean:
	rm -rf $(PPP_ASYNC_MODULE_IPK)
	rm -rf $(PPP_ASYNC_MODULE_BUILD_DIR)

