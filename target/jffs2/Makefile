include $(TOPDIR)/rules.mk

include ./jffs2root.mk

TARGETS:= $(BIN_DIR)/openwrt-generic-jffs2-4MB.trx $(BIN_DIR)/openwrt-generic-jffs2-8MB.trx

prepare: $(MTD_DIR)/.unpacked
compile: mtd
install: compile $(TARGETS)
clean: jffs2root-dirclean

$(BIN_DIR)/openwrt-generic-jffs2-4MB.trx:
	$(MKFS_JFFS2) --pad --little-endian --squash -e 0x10000 \
		-d $(TARGET_DIR) -o $(IMAGE)-4MB.jffs2
	PATH=$(TARGET_PATH) trx -o $@ $(BUILD_DIR)/loader.gz \
	 $(LINUX_IMAGE) -a 0x10000 $(IMAGE)-4MB.jffs2

$(BIN_DIR)/openwrt-generic-jffs2-8MB.trx:
	$(MKFS_JFFS2) --pad --little-endian --squash -e 0x20000 \
		-d $(TARGET_DIR) -o $(IMAGE)-8MB.jffs2
	PATH=$(TARGET_PATH) trx -o $@ $(BUILD_DIR)/loader.gz \
	 $(LINUX_IMAGE) -a 0x20000 $(IMAGE)-8MB.jffs2
	 
$(BIN_DIR)/openwrt-wrt54g-jffs2.bin: $(BIN_DIR)/openwrt-generic-jffs2-4MB.trx
	PATH=$(TARGET_PATH) addpattern -2 -i $< -o $@ -g
	$(SED) "1s,^W54S,W54G," $@
	
$(BIN_DIR)/openwrt-wrt54gs-jffs2.bin: $(BIN_DIR)/openwrt-generic-jffs2-8MB.trx
	PATH=$(TARGET_PATH) addpattern -2 -i $< -o $@ -g

$(BIN_DIR)/openwrt-motorola-jffs2-4MB.bin: $(BIN_DIR)/openwrt-generic-jffs2-4MB.trx
	PATH=$(TARGET_PATH) motorola-bin $< $@

$(BIN_DIR)/openwrt-motorola-jffs2-8MB.bin: $(BIN_DIR)/openwrt-generic-jffs2-8MB.trx
	PATH=$(TARGET_PATH) motorola-bin $< $@

