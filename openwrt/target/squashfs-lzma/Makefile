include $(TOPDIR)/rules.mk
include ./squashfslzmaroot.mk

TARGETS:=$(BIN_DIR)/openwrt-generic-squashfs.trx

prepare: $(SQUASHFSLZMA_DIR)/.unpacked
compile: squashfslzma
install: $(TARGETS)
clean: squashfslzmaroot-dirclean

$(BIN_DIR)/openwrt-generic-squashfs.trx:
	@mkdir -p $(TARGET_DIR)/jffs
	$(SQUASHFSLZMA_DIR)/squashfs-tools/mksquashfs-lzma $(TARGET_DIR) $(IMAGE).squashfslzma -noappend -root-owned -le
	PATH=$(TARGET_PATH) trx -o $@ $(BUILD_DIR)/loader.gz \
	 $(LINUX_IMAGE) $(IMAGE).squashfslzma
	 
