#############################################################
#
# squashfs target 
# 
#############################################################

SQUASHFS_DIR=$(BUILD_DIR)/squashfs2.1-r2
SQUASHFS_SOURCE=squashfs2.1-r2.tar.gz
SQUASHFS_SITE=http://dl.sourceforge.net/sourceforge/squashfs

$(DL_DIR)/$(SQUASHFS_SOURCE):
	$(WGET) -P $(DL_DIR) $(SQUASHFS_SITE)/$(SQUASHFS_SOURCE)

$(SQUASHFS_DIR)/.unpacked: $(DL_DIR)/$(SQUASHFS_SOURCE) 
	zcat $(DL_DIR)/$(SQUASHFS_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(SQUASHFS_DIR)/.unpacked

$(SQUASHFS_DIR)/squashfs-tools/mksquashfs: $(SQUASHFS_DIR)/.unpacked
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
	@rm -rf $(TARGET_DIR)/usr/man
	@rm -rf $(TARGET_DIR)/usr/info
	$(SQUASHFS_DIR)/squashfs-tools/mksquashfs $(TARGET_DIR) $(IMAGE).squashfs -noappend -root-owned -le

squashfsroot-source: squashfs-source

squashfsroot-clean:
	-$(MAKE) -C $(SQUASHFS_DIR) clean

squashfsroot-dirclean:
	rm -rf $(SQUASHFS_DIR)

openwrt-linux.trx.squashfs:
	$(BUILD_DIR)/trx -o openwrt-linux.trx.squashfs $(LINUX_DIR)/$(LINUX_BINLOC) $(IMAGE).squashfs

openwrt-gs-code.bin.squashfs: openwrt-linux.trx.squashfs
	$(BUILD_DIR)/addpattern -2 -i  openwrt-linux.trx.squashfs -o openwrt-gs-code.bin.squashfs -g

openwrt-g-code.bin.squashfs: openwrt-gs-code.bin.squashfs
	sed -e "1s,^W54S,W54G," < openwrt-gs-code.bin.squashfs > openwrt-g-code.bin.squashfs

openwrt-image-squashfs:	openwrt-g-code.bin.squashfs
