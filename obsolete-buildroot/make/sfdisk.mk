#############################################################
#
# sfdisk support
#
#############################################################
SFDISK_SOURCE=sfdisk.tar.bz2
SFDISK_SITE:=http://www.uclibc.org/
SFDISK_DIR=$(BUILD_DIR)/sfdisk


$(DL_DIR)/$(SFDISK_SOURCE):
	$(WGET) -P $(DL_DIR) $(SFDISK_SITE)/$(SFDISK_SOURCE)

$(SFDISK_DIR): $(DL_DIR)/$(SFDISK_SOURCE)
	bzcat $(DL_DIR)/$(SFDISK_SOURCE) | tar -C $(BUILD_DIR) -xvf -

$(SFDISK_DIR)/sfdisk: $(SFDISK_DIR)
	$(MAKE) CROSS=$(TARGET_CROSS) DEBUG=false -C $(SFDISK_DIR);
	-$(STRIP) $(SFDISK_DIR)/sfdisk;
	touch -c $(SFDISK_DIR)/sfdisk

$(TARGET_DIR)/sbin/sfdisk: $(SFDISK_DIR)/sfdisk
	cp $(SFDISK_DIR)/sfdisk $(TARGET_DIR)/sbin/sfdisk;
	touch -c $(TARGET_DIR)/sbin/sfdisk

sfdisk: uclibc $(TARGET_DIR)/sbin/sfdisk

sfdisk-source: $(DL_DIR)/$(SFDISK_SOURCE)

sfdisk-clean:
	rm -f $(TARGET_DIR)/sbin/sfdisk
	-$(MAKE) -C $(SFDISK_DIR) clean

sfdisk-dirclean:
	rm -rf $(SFDISK_DIR)

