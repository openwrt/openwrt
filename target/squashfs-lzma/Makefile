include $(TOPDIR)/rules.mk
include ./squashfslzmaroot.mk

SQUASHFS_TARGETS:=$(BIN_DIR)/openwrt-generic-squashfs.trx

prepare: $(SQUASHFSLZMA_DIR)/.unpacked
compile: squashfslzma
install: $(SQUASHFS_TARGETS)
clean: squashfslzmaroot-dirclean

$(BIN_DIR)/openwrt-generic-squashfs.trx:
	@mkdir -p $(TARGET_DIR)/jffs
	PATH=$(TARGET_PATH) mksquashfs-lzma $(TARGET_DIR) $(IMAGE).squashfslzma -noappend -root-owned -le
	PATH=$(TARGET_PATH) trx -o $@ $(BUILD_DIR)/loader.gz \
	 $(LINUX_IMAGE) $(IMAGE).squashfslzma
	 
