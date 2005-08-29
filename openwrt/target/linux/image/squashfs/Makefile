include $(TOPDIR)/rules.mk

SQUASHFSLZMA_DIR=$(BUILD_DIR)/squashfs2.2
SQUASHFSLZMA_SOURCE=squashfs2.2.tar.gz
SQUASHFSLZMA_SITE=@SF/squashfs
SQUASHFSLZMA_MD5SUM=9537f1bfa3efd5df722f1a85cd424414

$(DL_DIR)/$(SQUASHFSLZMA_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(SQUASHFSLZMA_SOURCE) $(SQUASHFSLZMA_MD5SUM) $(SQUASHFSLZMA_SITE)

$(SQUASHFSLZMA_DIR)/.unpacked: $(DL_DIR)/$(SQUASHFSLZMA_SOURCE) 
	zcat $(DL_DIR)/$(SQUASHFSLZMA_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	$(PATCH) $(SQUASHFSLZMA_DIR) ./patches
	touch $(SQUASHFSLZMA_DIR)/.unpacked

$(SQUASHFSLZMA_DIR)/squashfs-tools/mksquashfs-lzma: $(SQUASHFSLZMA_DIR)/.unpacked
	$(MAKE) -C $(SQUASHFSLZMA_DIR)/squashfs-tools mksquashfs-lzma LZMAPATH=$(BUILD_DIR)/lzma

$(STAGING_DIR)/bin/mksquashfs-lzma: $(SQUASHFSLZMA_DIR)/squashfs-tools/mksquashfs-lzma
	cp $< $@

source: $(DL_DIR)/$(SQUASHFSLZMA_SOURCE)
prepare: $(SQUASHFSLZMA_DIR)/.unpacked
compile: $(SQUASHFSLZMA_DIR)/squashfs-tools/mksquashfs-lzma $(STAGING_DIR)/bin/mksquashfs-lzma
install:
clean: 
	rm -rf $(SQUASHFSLZMA_DIR)
