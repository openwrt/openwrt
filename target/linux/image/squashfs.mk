squashfs-prepare:
	$(MAKE) -C squashfs prepare

squashfs-compile:
	$(MAKE) -C squashfs compile
	
squashfs-clean:
	$(MAKE) -C squashfs clean
	rm -f $(KDIR)/root.squashfs

$(KDIR)/root.squashfs:
	@mkdir -p $(KDIR)/root/jffs
	$(STAGING_DIR)/bin/mksquashfs-lzma $(KDIR)/root $@ -nopad -noappend -root-owned -le

squashfs-install: $(KDIR)/root.squashfs
	$(MAKE) -C $(BOARD) install KERNEL="$(KERNEL)" FS="squashfs"

squashfs-install-ib:
	mkdir -p $(IB_DIR)/staging_dir_$(ARCH)/bin
	cp $(STAGING_DIR)/bin/mksquashfs-lzma $(IB_DIR)/staging_dir_$(ARCH)/bin
	
prepare: squashfs-prepare
compile: squashfs-compile
install: squashfs-install
install-ib: squashfs-install-ib
clean: squashfs-clean
