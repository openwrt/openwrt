kmod-ipv6-ipk: ipkg-utils $(OPENWRT_IPK_DIR)/kmod-ipv6/CONTROL/*
	chmod a+x $(OPENWRT_IPK_DIR)/kmod-ipv6/CONTROL/rules
	cd $(OPENWRT_IPK_DIR)/kmod-ipv6 ; $(IPKG_BUILDPACKAGE)
	mv $(OPENWRT_IPK_DIR)/kmod-ipv6_*.ipk $(BUILD_DIR)