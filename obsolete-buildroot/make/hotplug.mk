#############################################################
#
# hotplug support
#
#############################################################
HOTPLUG_SOURCE=diethotplug-0.4.tar.gz
HOTPLUG_SITE=http://aleron.dl.sourceforge.net/sourceforge/linux-hotplug
HOTPLUG_DIR=$(BUILD_DIR)/diethotplug-0.4
HOTPLUG_PATCH=$(SOURCE_DIR)/hotplug.patch

$(DL_DIR)/$(HOTPLUG_SOURCE):
	$(WGET) -P $(DL_DIR) $(HOTPLUG_SITE)/$(HOTPLUG_SOURCE)

$(HOTPLUG_DIR): $(DL_DIR)/$(HOTPLUG_SOURCE) $(HOTPLUG_PATCH)
	zcat $(DL_DIR)/$(HOTPLUG_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	cat $(HOTPLUG_PATCH) | patch -p1 -d $(HOTPLUG_DIR)

$(HOTPLUG_DIR)/hotplug: $(HOTPLUG_DIR)
	$(MAKE) CROSS=$(TARGET_CROSS) DEBUG=false KLIBC=false \
	    KERNEL_INCLUDE_DIR=$(STAGING_DIR)/include \
	    TARGET_DIR=$(TARGET_DIR) -C $(HOTPLUG_DIR);
	$(STRIP) $(HOTPLUG_DIR)/hotplug;
	touch -c $(HOTPLUG_DIR)/hotplug

$(TARGET_DIR)/sbin/hotplug: $(HOTPLUG_DIR)/hotplug
	cp $(HOTPLUG_DIR)/hotplug $(TARGET_DIR)/sbin/hotplug;
	touch -c $(TARGET_DIR)/sbin/hotplug

hotplug: uclibc $(TARGET_DIR)/sbin/hotplug

hotplug-source: $(DL_DIR)/$(HOTPLUG_SOURCE)

hotplug-clean:
	rm -f $(TARGET_DIR)/sbin/hotplug
	-$(MAKE) -C $(HOTPLUG_DIR) clean

hotplug-dirclean:
	rm -rf $(HOTPLUG_DIR)

