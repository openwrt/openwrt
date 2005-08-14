JFFS2OPTS :=  --pad --little-endian --squash
#JFFS2OPTS += -Xlzo -msize -Xlzari

jffs2-prepare:
	$(MAKE) -C jffs2 prepare

jffs2-compile:
	$(MAKE) -C jffs2 compile

jffs2-clean:
	$(MAKE) -C jffs2 clean
	rm -f $(KDIR)/root.jffs2*

$(KDIR)/root.jffs2-4MB:
	@rm -rf $(KDIR)/root/jffs
	$(STAGING_DIR)/bin/mkfs.jffs2 $(JFFS2OPTS) -e 0x10000 -o $@ -d $(KDIR)/root

$(KDIR)/root.jffs2-8MB:
	@rm -rf $(KDIR)/root/jffs
	$(STAGING_DIR)/bin/mkfs.jffs2 $(JFFS2OPTS) -e 0x20000 -o $@ -d $(KDIR)/root

jffs2-install: $(KDIR)/root.jffs2-4MB $(KDIR)/root.jffs2-8MB
	$(MAKE) -C $(BOARD) install KERNEL="$(KERNEL)" FS="jffs2-4MB"
	$(MAKE) -C $(BOARD) install KERNEL="$(KERNEL)" FS="jffs2-8MB"

jffs2-install-ib:
	mkdir -p $(IB_DIR)/staging_dir_$(ARCH)/bin
	cp $(STAGING_DIR)/bin/mkfs.jffs2 $(IB_DIR)/staging_dir_$(ARCH)/bin

prepare: jffs2-prepare
compile: jffs2-compile
install: jffs2-install
install-ib: jffs2-install-ib
clean: jffs2-clean

