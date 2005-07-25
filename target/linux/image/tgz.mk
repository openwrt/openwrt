$(KDIR)/vmlinux.gz: $(KDIR)/vmlinux
	gzip -c -vf9 < $< > $@

$(KDIR)/rootfs.tar.gz: $(KDIR)/vmlinux.gz
	tar -cf $(KDIR)/rootfs.tar --owner=root --group=root -C $(KDIR)/ ./vmlinux.gz
	tar -rf $(KDIR)/rootfs.tar --owner=root --group=root -C $(KDIR)/root/ .
	gzip -f9 $(KDIR)/rootfs.tar

tgz-install: $(KDIR)/rootfs.tar.gz
	cp $(KDIR)/vmlinux.gz $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-vmlinux.gz
	cp $(KDIR)/rootfs.tar.gz $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-rootfs.tar.gz

prepare:
compile:
install: tgz-install
