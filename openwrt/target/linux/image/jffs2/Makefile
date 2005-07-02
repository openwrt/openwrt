include $(TOPDIR)/rules.mk

MTD_DIR:=$(BUILD_DIR)/mtd-20050122.orig
MTD_SOURCE=mtd_20050122.orig.tar.gz
MTD_SITE=http://ftp.debian.org/debian/pool/main/m/mtd
MTD_MD5SUM:=1f42c2cae08eb9e7b52d0c188f8d6338
MKFS_JFFS2=$(MTD_DIR)/util/mkfs.jffs2

$(DL_DIR)/$(MTD_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(MTD_SOURCE) $(MTD_MD5SUM) $(MTD_SITE)

$(MTD_DIR)/.unpacked: $(DL_DIR)/$(MTD_SOURCE)
	zcat $(DL_DIR)/$(MTD_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	$(PATCH) $(MTD_DIR) ./patches
	touch $(MTD_DIR)/.unpacked

$(MTD_DIR)/util/mkfs.jffs2: $(MTD_DIR)/.unpacked
	$(MAKE) LINUXDIR=$(LINUX_DIR) -C $(MTD_DIR)/util

$(STAGING_DIR)/bin/mkfs.jffs2: $(MTD_DIR)/util/mkfs.jffs2
	cp $< $@

source: $(DL_DIR)/$(MTD_SOURCE)
prepare: $(MTD_DIR)/.unpacked
compile: $(MTD_DIR)/util/mkfs.jffs2 $(STAGING_DIR)/bin/mkfs.jffs2
install: 
clean:
	rm -rf $(MTD_DIR)

