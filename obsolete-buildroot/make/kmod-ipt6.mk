kmod-ipt6-ipk: ipkg-utils $(OPENWRT_IPK_DIR)/kmod-ipt6/CONTROL/*
	chmod a+x $(OPENWRT_IPK_DIR)/kmod-ipt6/CONTROL/rules
	cd $(OPENWRT_IPK_DIR)/kmod-ipt6 ; $(IPKG_BUILDPACKAGE)
	mv $(OPENWRT_IPK_DIR)/kmod-ipt6_*.ipk $(BUILD_DIR)