## nfs-modules

NFS_MODULES_VERSION := $(SNAPSHOT)
NFS_MODULES_RELEASE := 1

NFS_MODULES_BUILD_DIR := $(BUILD_DIR)/nfs-modules_$(NFS_MODULES_VERSION)-$(NFS_MODULES_RELEASE)
NFS_MODULES_IPK_DIR := $(OPENWRT_IPK_DIR)/nfs-modules
NFS_MODULES_IPK := $(NFS_MODULES_BUILD_DIR)_$(ARCH).ipk


$(NFS_MODULES_BUILD_DIR)/CONTROL/control: $(BASE_DIR)/openwrt-kmodules.tar.bz2
	rm -rf $(NFS_MODULES_BUILD_DIR)
	mkdir -p $(NFS_MODULES_BUILD_DIR)/lib/
	bzcat $(BASE_DIR)/openwrt-kmodules.tar.bz2 | tar -C $(NFS_MODULES_BUILD_DIR)/lib/ -xvf - \
	 modules/$(LINUX_VERSION)/kernel/fs/lockd/lockd.o \
	 modules/$(LINUX_VERSION)/kernel/fs/nfs/nfs.o \
	 modules/$(LINUX_VERSION)/kernel/net/sunrpc/sunrpc.o
	mv -f \
	 $(NFS_MODULES_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/kernel/fs/lockd/*.o \
	 $(NFS_MODULES_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/
	mv -f \
	 $(NFS_MODULES_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/kernel/fs/nfs/*.o \
	 $(NFS_MODULES_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/
	mv -f \
	 $(NFS_MODULES_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/kernel/net/sunrpc/*.o \
	 $(NFS_MODULES_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/
	rm -rf \
	 $(NFS_MODULES_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/kernel
	chmod 0755 $(NFS_MODULES_BUILD_DIR)/lib
	chmod 0755 $(NFS_MODULES_BUILD_DIR)/lib/modules
	chmod 0755 $(NFS_MODULES_BUILD_DIR)/lib/modules/$(LINUX_VERSION)
	chmod 0755 $(NFS_MODULES_BUILD_DIR)/lib/modules/$(LINUX_VERSION)/*
	cp -a $(NFS_MODULES_IPK_DIR)/CONTROL $(NFS_MODULES_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(NFS_MODULES_VERSION)-$(NFS_MODULES_RELEASE)/" $(NFS_MODULES_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(NFS_MODULES_BUILD_DIR)/CONTROL/control

	touch $(NFS_MODULES_BUILD_DIR)/CONTROL/control


$(NFS_MODULES_IPK): $(NFS_MODULES_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(NFS_MODULES_BUILD_DIR)


nfs-modules-ipk: ipkg-utils $(NFS_MODULES_IPK)

nfs-modules-clean:
	rm -rf $(NFS_MODULES_IPK)
	rm -rf $(NFS_MODULES_BUILD_DIR)

