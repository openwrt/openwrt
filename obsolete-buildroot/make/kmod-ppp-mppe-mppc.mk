kmod-ppp-mppe-mppc-ipk: ipkg-utils $(OPENWRT_IPK_DIR)/kmod-ppp-mppe-mppc/CONTROL/*
	chmod a+x $(OPENWRT_IPK_DIR)/kmod-ppp-mppe-mppc/CONTROL/rules
	cd $(OPENWRT_IPK_DIR)/kmod-ppp-mppe-mppc ; $(IPKG_BUILDPACKAGE)
	mv $(OPENWRT_IPK_DIR)/kmod-ppp-mppe-mppc_*.ipk $(BUILD_DIR)