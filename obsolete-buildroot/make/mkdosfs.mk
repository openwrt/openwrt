#############################################################
#
# mkdosfs
#
#############################################################
MKDOSFS_SOURCE=dosfstools-2.8.src.tar.gz
MKDOSFS_SITE=http://ftp.uni-erlangen.de/pub/Linux/LOCAL/dosfstools
MKDOSFS_DIR=$(BUILD_DIR)/dosfstools-2.8
MKDOSFS_CAT:=zcat
MKDOSFS_BINARY:=mkdosfs/mkdosfs
MKDOSFS_TARGET_BINARY:=sbin/mkdosfs
ifeq ($(strip $(BUILD_WITH_LARGEFILE)),true)
MKDOSFS_CFLAGS="-Os -g -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64"
else
MKDOSFS_CFLAGS="-Os -g"
endif

$(DL_DIR)/$(MKDOSFS_SOURCE):
	 $(WGET) -P $(DL_DIR) $(MKDOSFS_SITE)/$(MKDOSFS_SOURCE)

mkdosfs-source: $(DL_DIR)/$(MKDOSFS_SOURCE)

$(MKDOSFS_DIR)/.unpacked: $(DL_DIR)/$(MKDOSFS_SOURCE)
	$(MKDOSFS_CAT) $(DL_DIR)/$(MKDOSFS_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(MKDOSFS_DIR)/.unpacked

$(MKDOSFS_DIR)/$(MKDOSFS_BINARY): $(MKDOSFS_DIR)/.unpacked
	$(MAKE) CFLAGS=$(MKDOSFS_CFLAGS) CC=$(TARGET_CC) -C $(MKDOSFS_DIR);
	$(STRIP) $(MKDOSFS_DIR)/mkdosfs/mkdosfs;
	touch -c $(MKDOSFS_DIR)/mkdosfs/mkdosfs

$(TARGET_DIR)/$(MKDOSFS_TARGET_BINARY): $(MKDOSFS_DIR)/$(MKDOSFS_BINARY)
	cp -a $(MKDOSFS_DIR)/$(MKDOSFS_BINARY) $(TARGET_DIR)/$(MKDOSFS_TARGET_BINARY)
	touch -c $(TARGET_DIR)/sbin/mkdosfs

mkdosfs: uclibc $(TARGET_DIR)/$(MKDOSFS_TARGET_BINARY)

mkdosfs-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(MKDOSFS_DIR) uninstall
	-$(MAKE) -C $(MKDOSFS_DIR) clean

mkdosfs-dirclean:
	rm -rf $(MKDOSFS_DIR)

