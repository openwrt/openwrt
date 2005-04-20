include $(TOPDIR)/rules.mk

include ./jffs2root.mk

JFFS2_TARGETS:= $(BIN_DIR)/openwrt-generic-jffs2-4MB.trx $(BIN_DIR)/openwrt-generic-jffs2-8MB.trx
JFFS2OPTS :=  --pad --little-endian --squash -d $(TARGET_DIR)
#JFFS2OPTS += -Xlzo -msize -Xlzari

prepare: $(MTD_DIR)/.unpacked
compile: mtd
install: compile $(JFFS2_TARGETS)
clean: jffs2root-dirclean

$(BIN_DIR)/openwrt-generic-jffs2-4MB.trx:
	$(MKFS_JFFS2) $(JFFS2OPTS) -e 0x10000 -o $(IMAGE)-4MB.jffs2
	PATH=$(TARGET_PATH) trx -o $@ $(BUILD_DIR)/loader.gz \
	 $(LINUX_IMAGE) -a 0x10000 $(IMAGE)-4MB.jffs2

$(BIN_DIR)/openwrt-generic-jffs2-8MB.trx:
	$(MKFS_JFFS2) $(JFFS2OPTS) -e 0x20000 -o $(IMAGE)-8MB.jffs2
	PATH=$(TARGET_PATH) trx -o $@ $(BUILD_DIR)/loader.gz \
	 $(LINUX_IMAGE) -a 0x20000 $(IMAGE)-8MB.jffs2
	 
