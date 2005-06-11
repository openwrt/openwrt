squashfs-prepare:
	$(MAKE) -C squashfs prepare

squashfs-compile:
	$(MAKE) -C squashfs compile
	
squashfs-clean:
	$(MAKE) -C squashfs clean
	rm -f $(KDIR)/root.squashfs

$(KDIR)/root.squashfs:
	@mkdir -p $(KDIR)/root/jffs
	$(STAGING_DIR)/bin/mksquashfs-lzma $(KDIR)/root $@ -noappend -root-owned -le

prepare: squashfs-prepare
compile: squashfs-compile
install: $(KDIR)/root.squashfs
	$(MAKE) -C $(BOARD) install KERNEL="$(KERNEL)" FS="squashfs"

