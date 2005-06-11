#############################################################
#
# lzma target 
# 
#############################################################

include $(TOPDIR)/rules.mk

LZMA_DIR=$(BUILD_DIR)/lzma
LZMA_SOURCE=lzma406.zip
LZMA_SITE=http://www.7-zip.org/dl
LZMA_MD5=a09378411cba5f786b5c49c9c58496df

LZMA_LIB_DIR=$(LZMA_DIR)/SRC/7zip/Compress/LZMA_Lib
LZMA_ALONE_DIR=$(LZMA_DIR)/SRC/7zip/Compress/LZMA_Alone

TARGETS=$(LZMA_LIB_DIR)/liblzma.a $(LZMA_ALONE_DIR)/lzma

$(DL_DIR)/$(LZMA_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(LZMA_SOURCE) $(LZMA_MD5) $(LZMA_SITE)

$(LZMA_DIR)/.unpacked: $(DL_DIR)/$(LZMA_SOURCE) 
	@rm -rf $(LZMA_DIR) && mkdir -p $(LZMA_DIR)
	unzip -q $^ -d $(LZMA_DIR)
	patch -d $(LZMA_DIR) -p1 < lzma-406-zlib-stream.patch
	touch $(LZMA_DIR)/.unpacked

$(LZMA_LIB_DIR)/liblzma.a: $(LZMA_DIR)/.unpacked
	$(MAKE) -C $(LZMA_LIB_DIR)

$(LZMA_ALONE_DIR)/lzma: $(LZMA_DIR)/.unpacked
	$(MAKE) -C $(LZMA_ALONE_DIR)

lzma: $(TARGETS)
	cp -f $^ $(LZMA_DIR)

lzma-source: $(DL_DIR)/$(LZMA_SOURCE)

lzma-clean:
	-$(MAKE) -C $(LZMA_LIB_DIR) clean
	-$(MAKE) -C $(LZMA_ALONE_DIR) clean
	rm -f $(LZMA_DIR)/liblzma.a $(LZMA_DIR)/lzma

lzma-dirclean:
	rm -rf $(LZMA_DIR)

#############################################################

prepare: $(LZMA_DIR)/.unpacked
compile: lzma
install: lzma
	cp $(LZMA_DIR)/lzma $(STAGING_DIR)/bin/
clean: lzma-dirclean
