#############################################################
#
# genext2fs to build to target ext2 filesystems
#
#############################################################
GENEXT2_DIR=$(BUILD_DIR)/genext2fs-1.3
GENEXT2_SOURCE=genext2fs_1.3.orig.tar.gz
GENEXT2_SITE=http://ftp.debian.org/debian/pool/main/g/genext2fs
GENEXT2_PATCH=$(SOURCE_DIR)/genext2fs.patch

$(DL_DIR)/$(GENEXT2_SOURCE):
	$(WGET) -P $(DL_DIR) $(GENEXT2_SITE)/$(GENEXT2_SOURCE)

$(GENEXT2_DIR): $(DL_DIR)/$(GENEXT2_SOURCE) $(GENEXT2_PATCH)
	zcat $(DL_DIR)/$(GENEXT2_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	mv $(GENEXT2_DIR).orig $(GENEXT2_DIR)
	cat $(GENEXT2_PATCH) | patch -p1 -d $(GENEXT2_DIR)

$(GENEXT2_DIR)/genext2fs: $(GENEXT2_DIR)
	$(MAKE) CFLAGS="-Wall -O2 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE \
		-D_FILE_OFFSET_BITS=64" -C $(GENEXT2_DIR);
	touch -c $(GENEXT2_DIR)/genext2fs

genext2fs: $(GENEXT2_DIR)/genext2fs



#############################################################
#
# Build the ext2 root filesystem image
#
#############################################################

# How much KB we want to add to the calculated size for slack space
#GENEXT2_ADDTOROOTSIZE=4096
GENEXT2_ADDTOROOTSIZE=16384
GENEXT2_REALSIZE=$(subst total,, $(shell LANG=C du $(TARGET_DIR) -s -c -k | grep total )) 
GENEXT2_SIZE=$(shell expr $(GENEXT2_REALSIZE) + $(GENEXT2_ADDTOROOTSIZE))
# We currently add about 400 device nodes, so add that into the total
GENEXT2_INODES=$(shell expr $(shell find $(TARGET_DIR) | wc -l) + 400)
#GENEXT2_SIZE=100000

ext2root: genext2fs
	#-@find $(TARGET_DIR)/lib -type f -name \*.so\* | xargs $(STRIP) --strip-unneeded 2>/dev/null || true;
	-@find $(TARGET_DIR) -type f -perm +111 | xargs $(STRIP) 2>/dev/null || true;
	$(GENEXT2_DIR)/genext2fs -i $(GENEXT2_INODES) -b $(GENEXT2_SIZE) \
		-d $(TARGET_DIR) -q -D $(SOURCE_DIR)/device_table.txt $(IMAGE)

ext2root-source: $(DL_DIR)/$(GENEXT2_SOURCE)

ext2root-clean:
	-$(MAKE) -C $(GENEXT2_DIR) clean

ext2root-dirclean:
	rm -rf $(GENEXT2_DIR)

