#############################################################
#
# mtd provides us with mkfs.jffs2, to target JFFS2 filesystems
#
#############################################################

MTD_DIR:=$(BUILD_DIR)/mtd-20050122.orig
MTD_SOURCE=mtd_20050122.orig.tar.gz
MTD_SITE=http://ftp.debian.org/debian/pool/main/m/mtd
MKFS_JFFS2=$(shell which mkfs.jffs2 2>/dev/null || echo $(MTD_DIR)/util/mkfs.jffs2)

$(DL_DIR)/$(MTD_SOURCE):
	$(WGET) -P $(DL_DIR) $(MTD_SITE)/$(MTD_SOURCE)

$(MTD_DIR)/.unpacked: $(DL_DIR)/$(MTD_SOURCE)
	zcat $(DL_DIR)/$(MTD_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(MTD_DIR)/.unpacked

$(MTD_DIR)/util/mkfs.jffs2: $(MTD_DIR)/.unpacked
	$(MAKE) LINUXDIR=$(LINUX_DIR) -C $(MTD_DIR)/util

mtd: $(MKFS_JFFS2)


#############################################################
#
# Build the jffs2 root filesystem image
#
#############################################################

jffs2root: mtd
	#-@find $(TARGET_DIR)/lib -type f -name \*.so\* | xargs $(STRIP) --strip-unneeded 2>/dev/null || true;
	-@find $(TARGET_DIR) -type f -perm +111 | xargs $(STRIP) 2>/dev/null || true;
	@rm -rf $(TARGET_DIR)/usr/man
	@rm -rf $(TARGET_DIR)/usr/info
	$(MKFS_JFFS2) --pad --little-endian --squash -e 0x20000 \
		-d $(TARGET_DIR) -o $(IMAGE).jffs2

jffs2root-source: $(DL_DIR)/$(MTD_SOURCE)

jffs2root-clean:
	-$(MAKE) -C $(MTD_DIR) clean

jffs2root-dirclean:
	rm -rf $(MTD_DIR)

ifeq ($(strip $(BR2_TARGET_ROOTFS_JFFS2)),y)
TARGETS+=jffs2root openwrt-image
ROOTFS=jffs2
endif
