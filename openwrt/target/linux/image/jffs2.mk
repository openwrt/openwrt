ifneq ($(CONFIG_BIG_ENDIAN),y)
JFFS2OPTS :=  --pad --little-endian --squash
else
JFFS2OPTS :=  --pad --big-endian --squash
endif

define Image/mkfs/jffs2
	rm -rf $(BUILD_DIR)/root/jffs
	
	$(STAGING_DIR)/bin/mkfs.jffs2 $(JFFS2OPTS) -e 0x10000 -o $(KDIR)/root.jffs2-64k -d $(BUILD_DIR)/root
	$(STAGING_DIR)/bin/mkfs.jffs2 $(JFFS2OPTS) -e 0x20000 -o $(KDIR)/root.jffs2-128k -d $(BUILD_DIR)/root

	$(call Image/Build,jffs2-64k)
	$(call Image/Build,jffs2-128k)
endef

jffs2-source: FORCE
	$(MAKE) -C $(TOPDIR)/target/linux/image/jffs2 source

$(STAGING_DIR)/bin/mkfs.jffs2:
	$(MAKE) -C $(TOPDIR)/target/linux/image/jffs2 compile

jffs2-clean: FORCE
	$(MAKE) -C $(TOPDIR)/target/linux/image/jffs2 clean
	rm -f $(KDIR)/root.jffs2*

source: jffs2-source
compile-targets: $(STAGING_DIR)/bin/mkfs.jffs2
clean-targets: jffs2-clean

