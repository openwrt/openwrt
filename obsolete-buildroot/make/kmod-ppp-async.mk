kmod-ppp-async-ipk: ipkg-utils $(OPENWRT_IPK_DIR)/kmod-ppp-async/CONTROL/*
	chmod a+x $(OPENWRT_IPK_DIR)/kmod-ppp-async/CONTROL/rules
	cd $(OPENWRT_IPK_DIR)/kmod-ppp-async ; $(IPKG_BUILDPACKAGE)
	mv $(OPENWRT_IPK_DIR)/kmod-ppp-async_*.ipk $(BUILD_DIR)