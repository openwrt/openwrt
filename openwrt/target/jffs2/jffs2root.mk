#############################################################
#
# mtd provides us with mkfs.jffs2, to target JFFS2 filesystems
#
#############################################################

MTD_DIR:=$(BUILD_DIR)/mtd-20050122.orig
MTD_SOURCE=mtd_20050122.orig.tar.gz
MTD_SITE=http://ftp.debian.org/debian/pool/main/m/mtd
MTD_MD5SUM:=1f42c2cae08eb9e7b52d0c188f8d6338
MKFS_JFFS2=$(MTD_DIR)/util/mkfs.jffs2

$(DL_DIR)/$(MTD_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(MTD_SOURCE) $(MTD_MD5SUM) $(MTD_SITE)

$(MTD_DIR)/.unpacked: $(DL_DIR)/$(MTD_SOURCE)
	zcat $(DL_DIR)/$(MTD_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(MTD_DIR)/.unpacked

$(MTD_DIR)/util/mkfs.jffs2: $(MTD_DIR)/.unpacked
	$(MAKE) LINUXDIR=$(LINUX_DIR) -C $(MTD_DIR)/util

$(STAGING_DIR)/bin/mkfs.jffs2: $(MTD_DIR)/util/mkfs.jffs2
	cp $< $@

mtd: $(MTD_DIR)/util/mkfs.jffs2 $(STAGING_DIR)/bin/mkfs.jffs2


#############################################################
#
# Build the jffs2 root filesystem image
#
#############################################################

jffs2root-source: $(DL_DIR)/$(MTD_SOURCE)

jffs2root-clean:
	-$(MAKE) -C $(MTD_DIR) clean

jffs2root-dirclean:
	rm -rf $(MTD_DIR)
