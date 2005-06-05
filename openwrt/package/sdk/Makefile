# $Id$

include $(TOPDIR)/rules.mk

PKG_OS:=$(shell uname -s)
PKG_CPU:=$(shell uname -m)

PKG_RELEASE:=1
SDK_NAME:=OpenWrt-SDK-$(PKG_OS)-$(PKG_CPU)-$(PKG_RELEASE)
IB_NAME:=OpenWrt-ImageBuilder-$(PKG_OS)-$(PKG_CPU)-$(PKG_RELEASE)

SDK_BUILD_DIR:=$(BUILD_DIR)/$(SDK_NAME)
IB_BUILD_DIR:=$(BUILD_DIR)/$(IB_NAME)

all: compile

$(BIN_DIR)/$(SDK_NAME).tar.bz2:
	(cd $(STAGING_DIR); \
		rm -rf info man share; \
		cd usr; \
		rm -rf doc info man share; \
	)
	rm -rf $(SDK_BUILD_DIR)
	mkdir -p $(SDK_BUILD_DIR)/dl $(SDK_BUILD_DIR)/examples $(SDK_BUILD_DIR)/package
	cp -a $(STAGING_DIR) $(SCRIPT_DIR) $(TOPDIR)/docs $(SDK_BUILD_DIR)/
	cp -a $(TOPDIR)/package/strace $(TOPDIR)/package/iproute2 $(SDK_BUILD_DIR)/examples
	cp $(TOPDIR)/rules.mk $(SDK_BUILD_DIR)/
	cp ./files/Makefile.sdk $(SDK_BUILD_DIR)/Makefile
	cp ./files/README.SDK $(SDK_BUILD_DIR)/
	cp ./files/depend.mk $(SDK_BUILD_DIR)/package/
	cp $(TOPDIR)/package/rules.mk $(SDK_BUILD_DIR)/package/
	egrep '^BR2_(ARCH|WGET|STAGING|JLEVEL|LARGEFILE|TARGET_OPT)' $(TOPDIR)/.config > $(SDK_BUILD_DIR)/.config
	find $(SDK_BUILD_DIR) -name CVS | xargs rm -rf 
	(cd $(BUILD_DIR); \
		tar cfj $@ $(SDK_NAME); \
	)

$(BIN_DIR)/$(IB_NAME).tar.bz2:
	rm -rf $(IB_BUILD_DIR)
	mkdir -p $(IB_BUILD_DIR)/build
	(cd $(BUILD_DIR); \
		cp buildroot-kernel-image loader.gz $(IB_BUILD_DIR)/build; \
	)
	mkdir -p $(IB_BUILD_DIR)/utils
	(cd $(STAGING_DIR)/bin; \
		cp addpattern trx motorola-bin sed mkfs.jffs2 mksquashfs-lzma $(IB_BUILD_DIR)/utils; \
	)
	mkdir -p $(IB_BUILD_DIR)/include
	mkdir -p $(IB_BUILD_DIR)/packages
	(cd $(PACKAGE_DIR); \
		cp ipkg_* busybox_* kmod-brcm-* kmod-wlcompat* dnsmasq_* iptables_* dropbear_* kmod-diag_* \
		   wireless-tools_* zlib* bridge_* ppp_* ppp*pppoe* kmod-ppp* openwrt-utils* $(IB_BUILD_DIR)/packages/; \
	)
	egrep '^[^(include|prepare|compile|install)]' < $(TOPDIR)/target/jffs2/Makefile > $(IB_BUILD_DIR)/include/jffs2.mk
	egrep '^[^(include|prepare|compile|install)]' < $(TOPDIR)/target/squashfs-lzma/Makefile > $(IB_BUILD_DIR)/include/squashfs-lzma.mk
	cp ./files/README.ImageBuilder $(IB_BUILD_DIR)/
	cp ./files/Makefile.ib $(IB_BUILD_DIR)/Makefile
	cp -a $(TOPDIR)/target/default/target_skeleton $(IB_BUILD_DIR)/
	for target_dir in etc jffs dev proc tmp lib usr/lib usr/sbin; do \
		mkdir -p $(IB_BUILD_DIR)/target_skeleton/$$target_dir; \
	done
	cp -a ./files/sysconf $(IB_BUILD_DIR)/target_skeleton/etc/
	cp -a $(TARGET_DIR)/lib/*.so* $(IB_BUILD_DIR)/target_skeleton/lib/
	cp -a $(BUILD_DIR)/libshared/libshared.so $(IB_BUILD_DIR)/target_skeleton/usr/lib/
	cp -a $(BUILD_DIR)/libnvram/libnvram.so $(IB_BUILD_DIR)/target_skeleton/usr/lib/
	cp -a $(BUILD_DIR)/libnvram/nvram $(IB_BUILD_DIR)/target_skeleton/usr/sbin/
	cp -a $(BUILD_DIR)/linksys-wlconf/wlconf $(IB_BUILD_DIR)/target_skeleton/usr/sbin/
	-$(STRIP) $(IB_BUILD_DIR)/target_skeleton/usr/lib/* $(IB_BUILD_DIR)/target_skeleton/usr/sbin/*
	ln -sf /tmp/resolv.conf $(IB_BUILD_DIR)/target_skeleton/etc/resolv.conf
	ln -sf /tmp $(IB_BUILD_DIR)/target_skeleton/var
	find $(IB_BUILD_DIR) -name CVS | xargs rm -rf 
	(cd $(BUILD_DIR); \
		tar cfj $@ $(IB_NAME); \
	)

source:
prepare:
compile: $(BIN_DIR)/$(SDK_NAME).tar.bz2
# FIXME: image builder disabled for now. need to find a way to make it work cleanly with the new kernel structure
# $(BIN_DIR)/$(IB_NAME).tar.bz2
install:

clean:
	rm -rf $(SDK_BUILD_DIR) $(BIN_DIR)/$(SDK_NAME).tar.bz2 $(BIN_DIR)/$(IB_NAME).tar.bz2
