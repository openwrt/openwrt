ifneq ($(CONFIG_BIG_ENDIAN),y)
endian := le
else
endian := be
endif

squashfs-prepare: FORCE
	$(MAKE) -C squashfs prepare

squashfs-compile: prepare-targets
	$(MAKE) -C squashfs compile
	
squashfs-clean: FORCE
	$(MAKE) -C squashfs clean
	rm -f $(KDIR)/root.squashfs

$(KDIR)/root.squashfs: install-prepare
	@mkdir -p $(BUILD_DIR)/root/jffs
	$(STAGING_DIR)/bin/mksquashfs-lzma $(BUILD_DIR)/root $@ -nopad -noappend -root-owned -$(endian)
	
ifeq ($(IB),)
squashfs-install: compile-targets $(BOARD)-compile
endif

squashfs-install: $(KDIR)/root.squashfs FORCE
	$(MAKE) -C $(BOARD) install KERNEL="$(KERNEL)" FS="squashfs"

squashfs-install-ib: compile-targets
	mkdir -p $(IB_DIR)/staging_dir_$(ARCH)/bin
	$(CP) $(STAGING_DIR)/bin/mksquashfs-lzma $(IB_DIR)/staging_dir_$(ARCH)/bin
	
prepare-targets: squashfs-prepare
compile-targets: squashfs-compile
install-targets: squashfs-install
install-ib: squashfs-install-ib
clean: squashfs-clean
