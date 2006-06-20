ifneq ($(CONFIG_BIG_ENDIAN),y)
ENDIAN := le
else
ENDIAN := be
endif

squashfs-source: FORCE
	$(MAKE) -C $(TOPDIR)/target/linux/image/squashfs source

$(STAGING_DIR)/bin/mksquashfs-lzma:
	$(MAKE) -C $(TOPDIR)/target/linux/image/squashfs compile
	
squashfs-clean: FORCE
	$(MAKE) -C $(TOPDIR)/target/linux/image/squashfs clean
	rm -f $(KDIR)/root.squashfs

define Image/mkfs/squashfs
	@mkdir -p $(BUILD_DIR)/root/jffs
	$(STAGING_DIR)/bin/mksquashfs-lzma $(BUILD_DIR)/root $(KDIR)/root.squashfs -nopad -noappend -root-owned -$(ENDIAN)
	$(call Image/Build,squashfs)
endef

FILESYSTEMS += squashfs
compile-targets: $(STAGING_DIR)/bin/mksquashfs-lzma
clean-targets: squashfs-clean
