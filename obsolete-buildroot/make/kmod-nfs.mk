kmod-nfs-ipk: ipkg-utils $(OPENWRT_IPK_DIR)/kmod-nfs/CONTROL/*
	chmod a+x $(OPENWRT_IPK_DIR)/kmod-nfs/CONTROL/rules
	cd $(OPENWRT_IPK_DIR)/kmod-nfs ; $(IPKG_BUILDPACKAGE)
	mv $(OPENWRT_IPK_DIR)/kmod-nfs_*.ipk $(BUILD_DIR)