#############################################################
#
# busybox
#
#############################################################

ifneq ($(strip $(USE_BUSYBOX_SNAPSHOT)),)
# Be aware that this changes daily....
BUSYBOX_DIR:=$(BUILD_DIR)/busybox
BUSYBOX_SOURCE:=busybox-$(strip $(USE_BUSYBOX_SNAPSHOT)).tar.bz2
BUSYBOX_SITE:=http://www.busybox.net/downloads/snapshots
else
BUSYBOX_DIR:=$(BUILD_DIR)/busybox-1.00-pre8
BUSYBOX_SOURCE:=busybox-1.00-pre8.tar.bz2
BUSYBOX_SITE:=http://www.busybox.net/downloads
endif
BUSYBOX_UNZIP=bzcat
BUSYBOX_CONFIG:=$(SOURCE_DIR)/openwrt/busybox/busybox.config

$(DL_DIR)/$(BUSYBOX_SOURCE):
	 $(WGET) -P $(DL_DIR) $(BUSYBOX_SITE)/$(BUSYBOX_SOURCE)

busybox-source: $(DL_DIR)/$(BUSYBOX_SOURCE) $(BUSYBOX_CONFIG)

$(BUSYBOX_DIR)/.configured: $(DL_DIR)/$(BUSYBOX_SOURCE) $(BUSYBOX_CONFIG)
	$(BUSYBOX_UNZIP) $(DL_DIR)/$(BUSYBOX_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	# Allow busybox patches.
	$(SOURCE_DIR)/patch-kernel.sh $(BUSYBOX_DIR) $(SOURCE_DIR)/openwrt/busybox/patches
	cp $(BUSYBOX_CONFIG) $(BUSYBOX_DIR)/.config
	$(SED) "s,^CROSS.*,CROSS=$(TARGET_CROSS)\n\
		PREFIX=$(TARGET_DIR),;" $(BUSYBOX_DIR)/Rules.mak
ifeq ($(strip $(BUILD_WITH_LARGEFILE)),true)
	$(SED) "s/^.*CONFIG_LFS.*/CONFIG_LFS=y/;" $(BUSYBOX_DIR)/.config
else
	$(SED) "s/^.*CONFIG_LFS.*/CONFIG_LFS=n/;" $(BUSYBOX_DIR)/.config
endif
	$(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" -C $(BUSYBOX_DIR) oldconfig
	touch $(BUSYBOX_DIR)/.configured

busybox-unpack: $(BUSYBOX_DIR)/.configured

$(BUSYBOX_DIR)/busybox: $(BUSYBOX_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" PREFIX="$(TARGET_DIR)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS) -fomit-frame-pointer" -C $(BUSYBOX_DIR)

$(TARGET_DIR)/bin/busybox: $(BUSYBOX_DIR)/busybox
	$(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" PREFIX="$(TARGET_DIR)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" -C $(BUSYBOX_DIR) install
	# Just in case
	-chmod a+x $(TARGET_DIR)/usr/share/udhcpc/default.script

busybox: uclibc $(TARGET_DIR)/bin/busybox

busybox-clean:
	rm -f $(TARGET_DIR)/bin/busybox
	-$(MAKE) -C $(BUSYBOX_DIR) clean

busybox-dirclean:
	rm -rf $(BUSYBOX_DIR)
