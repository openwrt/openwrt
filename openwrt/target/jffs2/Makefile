include $(TOPDIR)/rules.mk

MTD_DIR:=$(BUILD_DIR)/mtd-20050122.orig
MTD_SOURCE=mtd_20050122.orig.tar.gz
MTD_SITE=http://ftp.debian.org/debian/pool/main/m/mtd
MTD_MD5SUM:=1f42c2cae08eb9e7b52d0c188f8d6338
MKFS_JFFS2=$(MTD_DIR)/util/mkfs.jffs2
JFFS2OPTS :=  --pad --little-endian --squash 
#JFFS2OPTS += -Xlzo -msize -Xlzari

$(DL_DIR)/$(MTD_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(MTD_SOURCE) $(MTD_MD5SUM) $(MTD_SITE)

$(MTD_DIR)/.unpacked: $(DL_DIR)/$(MTD_SOURCE)
	zcat $(DL_DIR)/$(MTD_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(MTD_DIR)/.unpacked

$(MTD_DIR)/util/mkfs.jffs2: $(MTD_DIR)/.unpacked
	$(MAKE) LINUXDIR=$(LINUX_DIR) -C $(MTD_DIR)/util

$(STAGING_DIR)/bin/mkfs.jffs2: $(MTD_DIR)/util/mkfs.jffs2
	cp $< $@

define jffs2-4MB_template
	@rm -rf $(1)/root/jffs
	$(MKFS_JFFS2) $(JFFS2OPTS) -e 0x10000 -o $(1)/root.jffs2-4MB -d $(1)/root
	PATH=$(TARGET_PATH) trx -o $@ $(BUILD_DIR)/loader.gz \
	 $(1)/kernel-image -a 0x10000 $(1)/root.jffs2-4MB
endef

define jffs2-8MB_template
	@rm -rf $(1)/root/jffs
	$(MKFS_JFFS2) $(JFFS2OPTS) -e 0x20000 -o $(1)/root.jffs2-8MB -d $(1)/root
	PATH=$(TARGET_PATH) trx -o $@ $(BUILD_DIR)/loader.gz \
	 $(1)/kernel-image -a 0x10000 $(1)/root.jffs2-8MB
endef

FILESYSTEMS:=jffs2-4MB jffs2-8MB
include ../image.mk

source: $(DL_DIR)/$(MTD_SOURCE)
prepare: $(MTD_DIR)/.unpacked
compile: $(MTD_DIR)/util/mkfs.jffs2 $(STAGING_DIR)/bin/mkfs.jffs2
install: 
clean:
	rm -rf $(MTD_DIR)

