#############################################################
#
# squashfs target 
# 
#############################################################

SQUASHFSLZMA_DIR=$(BUILD_DIR)/squashfs2.1-r2
SQUASHFSLZMA_SOURCE=squashfs2.1-r2.tar.gz
SQUASHFSLZMA_SITE=@SF/squashfs
SQUASHFSLZMA_MD5SUM=ffa24eff13b2ca51ed6990d15237e4c1

$(DL_DIR)/$(SQUASHFSLZMA_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(SQUASHFSLZMA_SOURCE) $(SQUASHFSLZMA_MD5SUM) $(SQUASHFSLZMA_SITE)

$(SQUASHFSLZMA_DIR)/.unpacked: $(DL_DIR)/$(SQUASHFSLZMA_SOURCE) 
	zcat $(DL_DIR)/$(SQUASHFSLZMA_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	patch -d $(SQUASHFSLZMA_DIR) -p1 < squashfs2.0-tools-lzma.patch
	touch $(SQUASHFSLZMA_DIR)/.unpacked

$(SQUASHFSLZMA_DIR)/squashfs-tools/mksquashfs-lzma: $(SQUASHFSLZMA_DIR)/.unpacked
	$(MAKE) -C $(SQUASHFSLZMA_DIR)/squashfs-tools mksquashfs-lzma LZMAPATH=$(BUILD_DIR)/lzma

$(STAGING_DIR)/bin/mksquashfs-lzma: $(SQUASHFSLZMA_DIR)/squashfs-tools/mksquashfs-lzma
	cp $< $@

squashfslzma: $(SQUASHFSLZMA_DIR)/squashfs-tools/mksquashfs-lzma $(STAGING_DIR)/bin/mksquashfs-lzma
	
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

squashfslzmaroot-source: squashfslzma-source

squashfslzmaroot-dirclean:
	rm -rf $(SQUASHFSLZMA_DIR)
