#############################################################
#
# squashfs target 
# 
#############################################################

SQUASHFSLZMA_DIR=$(BUILD_DIR)/squashfs2.1-r2-lzma
SQUASHFSLZMA_SOURCE=squashfs2.1-r2-lzma.tar.gz
SQUASHFSLZMA_SITE=http://openwrt.openbsd-geek.de

$(DL_DIR)/$(SQUASHFSLZMA_SOURCE):
	$(WGET) -P $(DL_DIR) $(SQUASHFSLZMA_SITE)/$(SQUASHFSLZMA_SOURCE)

$(SQUASHFSLZMA_DIR)/.unpacked: $(DL_DIR)/$(SQUASHFSLZMA_SOURCE) 
	zcat $(DL_DIR)/$(SQUASHFSLZMA_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(SQUASHFSLZMA_DIR)/.unpacked

$(SQUASHFSLZMA_DIR)/squashfs-tools/mksquashfs: $(SQUASHFSLZMA_DIR)/.unpacked
	$(MAKE) -C $(SQUASHFSLZMA_DIR)/lzma/SRC/7zip/Compress/LZMA_Lib 
	$(MAKE) -C $(SQUASHFSLZMA_DIR)/squashfs-tools mksquashfs-lzma

squashfslzma: $(SQUASHFSLZMA_DIR)/squashfs-tools/mksquashfs
	
squashfslzma-source: $(DL_DIR)/$(SQUASHFSLZMA_SOURCE)

squashfslzma-clean:
	-$(MAKE) -C $(SQUASHFSLZMA_DIR)/squashfs-tools clean

squashfslzma-dirclean:
	rm -rf $(SQUASHFSLZMA_DIR)


#############################################################
#
# Build the squashfs root filesystem image
#
#############################################################

squashfslzmaroot: squashfslzma
	@rm -rf $(TARGET_DIR)/usr/man
	@rm -rf $(TARGET_DIR)/usr/info
	$(SQUASHFSLZMA_DIR)/squashfs-tools/mksquashfs-lzma $(TARGET_DIR) $(IMAGE).squashfslzma -noappend -root-owned -le

squashfslzmaroot-source: squashfslzma-source

squashfslzmaroot-clean:
	-$(MAKE) -C $(SQUASHFSLZMA_DIR) clean

squashfslzmaroot-dirclean:
	rm -rf $(SQUASHFSLZMA_DIR)
