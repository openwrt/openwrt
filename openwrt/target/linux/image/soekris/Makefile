include $(TOPDIR)/rules.mk

KDIR:=$(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)


ifeq ($(FS),jffs2-8MB)

$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FS).img: $(KDIR)/root.$(FS)
	cp $^ $@

install: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(FS).img

endif


ifeq ($(FS),tgz)

$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-rootfs.tar.gz: $(KDIR)/root.tar.gz
	cp $^ $@

install: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-rootfs.tar.gz

endif


$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-vmlinuz: $(KDIR)/bzImage
	cp $^ $@


clean:
prepare:
compile:
install: $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-vmlinuz
install-ib:
	mkdir -p $(IB_DIR)/build_$(ARCH)/linux-$(KERNEL)-$(BOARD)
	cp -dpR $(KDIR)/bzImage $(IB_DIR)/build_$(ARCH)/linux-$(KERNEL)-$(BOARD)/
