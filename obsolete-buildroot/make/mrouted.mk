#############################################################
#
# mrouted
#
#
#############################################################
MROUTED_SOURCE:=mrouted_3.9-beta3.orig.tar.gz
MROUTED_SITE:=http://ftp.debian.org/debian/pool/non-free/m/mrouted
MROUTED_DIR:=$(BUILD_DIR)/mrouted-3.9-beta3.orig
MROUTED_CAT:=zcat
#MROUTED_PATCH:=$(SOURCE_DIR)/mrouted_3.9-beta3-1.1.diff
MROUTED_PATCH:=mrouted_3.9-beta3-1.1.diff.gz
MROUTED_BINARY:=mrouted
MROUTED_TARGET_BINARY:=usr/sbin/mrouted

$(DL_DIR)/$(MROUTED_SOURCE):
	 $(WGET) -P $(DL_DIR) $(MROUTED_SITE)/$(MROUTED_SOURCE)

$(DL_DIR)/$(MROUTED_PATCH):
	 $(WGET) -P $(DL_DIR) $(MROUTED_SITE)/$(MROUTED_PATCH)

mrouted-source: $(DL_DIR)/$(MROUTED_SOURCE) $(DL_DIR)/$(MROUTED_PATCH)

$(MROUTED_DIR)/.unpacked: mrouted-source
	$(MROUTED_CAT) $(DL_DIR)/$(MROUTED_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	$(MROUTED_CAT) $(DL_DIR)/$(MROUTED_PATCH) | patch -p1 -d $(MROUTED_DIR)
	$(SOURCE_DIR)/patch-kernel.sh $(MROUTED_DIR) $(SOURCE_DIR) mrouted-*.patch
	touch $(MROUTED_DIR)/.unpacked

$(MROUTED_DIR)/$(MROUTED_BINARY): $(MROUTED_DIR)/.unpacked
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) CC=$(TARGET_CC) -C $(MROUTED_DIR)

$(TARGET_DIR)/$(MROUTED_TARGET_BINARY): $(MROUTED_DIR)/$(MROUTED_BINARY)
	cp -a $(MROUTED_DIR)/$(MROUTED_BINARY) $(TARGET_DIR)/$(MROUTED_TARGET_BINARY)

mrouted: uclibc $(TARGET_DIR)/$(MROUTED_TARGET_BINARY)

mrouted-clean:
	rm -f $(TARGET_DIR)/$(MROUTED_TARGET_BINARY)
	-$(MAKE) -C $(MROUTED_DIR) clean

mrouted-dirclean:
	rm -rf $(MROUTED_DIR)

