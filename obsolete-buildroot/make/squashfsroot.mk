#############################################################
#
# mksquashfs to build to target squashfs filesystems
#
#############################################################
SQUASHFS_DIR=$(BUILD_DIR)/squashfs1.3r3
SQUASHFS_SOURCE=squashfs1.3r3.tar.gz
SQUASHFS_SITE=http://aleron.dl.sourceforge.net/sourceforge/squashfs

$(DL_DIR)/$(SQUASHFS_SOURCE):
	 $(WGET) -P $(DL_DIR) $(SQUASHFS_SITE)/$(SQUASHFS_SOURCE)

$(SQUASHFS_DIR): $(DL_DIR)/$(SQUASHFS_SOURCE) #$(SQUASHFS_PATCH)
	zcat $(DL_DIR)/$(SQUASHFS_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	$(SOURCE_DIR)/patch-kernel.sh $(SQUASHFS_DIR) $(SOURCE_DIR) squashfs.patch

$(SQUASHFS_DIR)/squashfs-tools/mksquashfs: $(SQUASHFS_DIR)
	$(MAKE) -C $(SQUASHFS_DIR)/squashfs-tools;

squashfs: $(SQUASHFS_DIR)/squashfs-tools/mksquashfs

squashfs-source: $(DL_DIR)/$(SQUASHFS_SOURCE)

squashfs-clean:
	-$(MAKE) -C $(SQUASHFS_DIR)/squashfs-tools clean

squashfs-dirclean:
	rm -rf $(SQUASHFS_DIR)

#############################################################
#
# Build the squashfs root filesystem image
#
#############################################################

squashfsroot: squashfs
	#-@find $(TARGET_DIR)/lib -type f -name \*.so\* | xargs $(STRIP) --strip-unneeded 2>/dev/null || true;
	-@find $(TARGET_DIR) -type f -perm +111 | xargs $(STRIP) 2>/dev/null || true;
	@rm -rf $(TARGET_DIR)/usr/man
	@rm -rf $(TARGET_DIR)/usr/info
	#$(SQUASHFS_DIR)/squashfs-tools/mksquashfs -q -D $(SOURCE_DIR)/device_table.txt $(TARGET_DIR) $(IMAGE)
	$(SQUASHFS_DIR)/squashfs-tools/mksquashfs $(TARGET_DIR) $(IMAGE) -noappend -root-owned

squashfsroot-source: squashfs-source

squashfsroot-clean:
	-$(MAKE) -C $(SQUASHFS_DIR) clean

squashfsroot-dirclean:
	rm -rf $(SQUASHFS_DIR)

