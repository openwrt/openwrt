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
	
prepare: squashfs-prepare
compile: squashfs-compile
install: squashfs-install
