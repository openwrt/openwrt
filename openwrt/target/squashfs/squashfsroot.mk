#############################################################
#
# squashfs target 
# 
#############################################################

ROOTFSTYPE=squashfs

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
	$(SQUASHFS_DIR)/squashfs-tools/mksquashfs $(TARGET_DIR) $(IMAGE).$(ROOTFSTYPE) -noappend -root-owned -le

squashfsroot-source: squashfs-source

squashfsroot-clean:
	-$(MAKE) -C $(SQUASHFS_DIR) clean

squashfsroot-dirclean:
	rm -rf $(SQUASHFS_DIR)

openwrt-linux.trx.$(ROOTFSTYPE):
	$(BUILD_DIR)/trx -o openwrt-linux.trx.$(ROOTFSTYPE) $(LINUX_DIR)/$(LINUX_BINLOC) $(IMAGE).$(ROOTFSTYPE)

openwrt-gs-code.bin.$(ROOTFSTYPE): openwrt-linux.trx.$(ROOTFSTYPE)
	$(BUILD_DIR)/addpattern -2 -i  openwrt-linux.trx.$(ROOTFSTYPE) -o openwrt-gs-code.bin.$(ROOTFSTYPE) -g

openwrt-g-code.bin.$(ROOTFSTYPE): openwrt-gs-code.bin.$(ROOTFSTYPE)
	sed -e "1s,^W54S,W54G," < openwrt-gs-code.bin.$(ROOTFSTYPE) > openwrt-g-code.bin.$(ROOTFSTYPE)

openwrt-image:	openwrt-g-code.bin.$(ROOTFSTYPE)
