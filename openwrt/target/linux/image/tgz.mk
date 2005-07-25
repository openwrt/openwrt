$(KDIR)/vmlinux.gz: $(KDIR)/vmlinux
	gzip -c -vf9 < $< > $@

$(KDIR)/rootfs.tar.gz: $(KDIR)/vmlinux.gz
	tar -cf $(KDIR)/rootfs.tar --owner=root --group=root -C $(KDIR)/ ./vmlinux.gz
	tar -rf $(KDIR)/rootfs.tar --owner=root --group=root -C $(KDIR)/root/ .
	gzip -f9 $(KDIR)/rootfs.tar

tgz-install: $(KDIR)/rootfs.tar.gz
	$(MAKE) -C $(BOARD) install KERNEL="$(KERNEL)" FS="tgz"

prepare:
compile:
install: tgz-install
