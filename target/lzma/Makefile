#############################################################
#
# lzma target 
# 
#############################################################

include $(TOPDIR)/rules.mk

LZMA_DIR=$(BUILD_DIR)/lzma
LZMA_SOURCE=lzma432.tar.bz2
LZMA_SITE=@SF/sevenzip
LZMA_MD5=155c1ebce5bc6710ae7ecc926226d9d7

LZMA_LIB_DIR=$(LZMA_DIR)/C/7zip/Compress/LZMA_Lib
LZMA_ALONE_DIR=$(LZMA_DIR)/C/7zip/Compress/LZMA_Alone

TARGETS=$(LZMA_LIB_DIR)/liblzma.a $(LZMA_ALONE_DIR)/lzma

$(DL_DIR)/$(LZMA_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(LZMA_SOURCE) $(LZMA_MD5) $(LZMA_SITE) $(MAKE_TRACE)

$(LZMA_DIR)/.unpacked: $(DL_DIR)/$(LZMA_SOURCE) 
	@rm -rf $(LZMA_DIR) && mkdir -p $(LZMA_DIR)
	tar xjf $^ -C $(LZMA_DIR)
	patch -d $(LZMA_DIR) -p1 < lzma-zlib.patch $(MAKE_TRACE)
	touch $(LZMA_DIR)/.unpacked
	
$(LZMA_ALONE_DIR)/lzma $(LZMA_LIB_DIR)/liblzma.a: $(LZMA_DIR)/.built
$(LZMA_DIR)/.built: $(LZMA_DIR)/.unpacked
	$(MAKE) -C $(LZMA_LIB_DIR) $(MAKE_TRACE)
	$(MAKE) -f makefile.gcc -C $(LZMA_ALONE_DIR) $(MAKE_TRACE)
	touch $@

lzma: $(TARGETS)
	$(CP) $^ $(LZMA_DIR)

lzma-source: $(DL_DIR)/$(LZMA_SOURCE)

lzma-clean:
	-$(MAKE) -C $(LZMA_LIB_DIR) clean $(MAKE_TRACE)
	-$(MAKE) -C $(LZMA_ALONE_DIR) clean $(MAKE_TRACE)
	rm -f $(LZMA_DIR)/liblzma.a $(LZMA_DIR)/lzma

lzma-dirclean:
	rm -rf $(LZMA_DIR)

#############################################################

prepare: $(LZMA_DIR)/.unpacked
compile: lzma
install: lzma
	$(CP) $(LZMA_DIR)/lzma $(STAGING_DIR)/bin/
clean: lzma-dirclean
