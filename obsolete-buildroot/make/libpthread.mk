libpthread-ipk: ipkg-utils $(OPENWRT_IPK_DIR)/libpthread/CONTROL/*
	chmod a+x $(OPENWRT_IPK_DIR)/libpthread/CONTROL/rules
	cd $(OPENWRT_IPK_DIR)/libpthread ; $(IPKG_BUILDPACKAGE)
	mv $(OPENWRT_IPK_DIR)/libpthread_*.ipk $(BUILD_DIR)