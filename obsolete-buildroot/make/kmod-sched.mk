kmod-sched-ipk: ipkg-utils $(OPENWRT_IPK_DIR)/kmod-sched/CONTROL/*
	chmod a+x $(OPENWRT_IPK_DIR)/kmod-sched/CONTROL/rules
	cd $(OPENWRT_IPK_DIR)/kmod-sched ; $(IPKG_BUILDPACKAGE)
	mv $(OPENWRT_IPK_DIR)/kmod-sched_*.ipk $(BUILD_DIR)