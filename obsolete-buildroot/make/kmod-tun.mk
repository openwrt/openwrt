kmod-tun-ipk: ipkg-utils $(OPENWRT_IPK_DIR)/kmod-tun/CONTROL/*
	chmod a+x $(OPENWRT_IPK_DIR)/kmod-tun/CONTROL/rules
	cd $(OPENWRT_IPK_DIR)/kmod-tun ; $(IPKG_BUILDPACKAGE)
	mv $(OPENWRT_IPK_DIR)/kmod-tun_*.ipk $(BUILD_DIR)