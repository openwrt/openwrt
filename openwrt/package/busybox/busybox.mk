# $Id$

ifeq ($(strip $(BR2_PACKAGE_BUSYBOX_SNAPSHOT)),y)
# Be aware that this changes daily....
BUSYBOX_DIR:=$(BUILD_DIR)/busybox
BUSYBOX_SOURCE:=busybox-snapshot.tar.bz2
BUSYBOX_SITE:=http://www.busybox.net/downloads/snapshots
else
BUSYBOX_VER:=1.00
BUSYBOX_MD5:=5c7ea9b5d3b3677dfcdcec38b281a756
BUSYBOX_DIR:=$(BUILD_DIR)/busybox-$(BUSYBOX_VER)
BUSYBOX_SOURCE:=busybox-$(BUSYBOX_VER).tar.bz2
BUSYBOX_SITE:=http://www.busybox.net/downloads
endif
BUSYBOX_UNZIP=bzcat

$(DL_DIR)/$(BUSYBOX_SOURCE):
	 $(SCRIPT_DIR)/download.pl $(DL_DIR) $(BUSYBOX_SOURCE) $(BUSYBOX_MD5) $(BUSYBOX_SITE)

busybox-source: $(DL_DIR)/$(BUSYBOX_SOURCE) $(BUSYBOX_CONFIG)

$(BUSYBOX_DIR)/.unpacked: $(DL_DIR)/$(BUSYBOX_SOURCE)
	$(BUSYBOX_UNZIP) $(DL_DIR)/$(BUSYBOX_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	# Allow busybox patches.
	$(PATCH) $(BUSYBOX_DIR) ./patches
	touch $(BUSYBOX_DIR)/.unpacked

$(BUSYBOX_DIR)/.configured: $(BUSYBOX_DIR)/.unpacked
	$(SCRIPT_DIR)/gen_busybox_config.pl $(TOPDIR)/.config > $(BUSYBOX_DIR)/.config
#	cp $(BUSYBOX_CONFIG) $(BUSYBOX_DIR)/.config
#	$(SED) "s,^CROSS.*,CROSS=$(TARGET_CROSS)\n\
#		PREFIX=$(TARGET_DIR),;" $(BUSYBOX_DIR)/Rules.mak
#ifeq ($(BR2_LARGEFILE),y)
#	$(SED) "s/^.*CONFIG_LFS.*/CONFIG_LFS=y/;" $(BUSYBOX_DIR)/.config
#else
#	$(SED) "s/^.*CONFIG_LFS.*/CONFIG_LFS=n/;" $(BUSYBOX_DIR)/.config
#	$(SED) "s/^.*FDISK_SUPPORT_LARGE_DISKS.*/FDISK_SUPPORT_LARGE_DISKS=n/;" $(BUSYBOX_DIR)/.config
#endif
	$(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" -C $(BUSYBOX_DIR) oldconfig
	touch $(BUSYBOX_DIR)/.configured

$(BUSYBOX_DIR)/busybox: $(BUSYBOX_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" PREFIX="$(TARGET_DIR)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" -C $(BUSYBOX_DIR)

$(TARGET_DIR)/bin/busybox: $(BUSYBOX_DIR)/busybox
	$(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" PREFIX="$(TARGET_DIR)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" -C $(BUSYBOX_DIR) install
	$(STRIP) $(TARGET_DIR)/bin/busybox
	# Just in case
	-chmod a+x $(TARGET_DIR)/usr/share/udhcpc/default.script

busybox: $(TARGET_DIR)/bin/busybox

busybox-clean:
	rm -f $(TARGET_DIR)/bin/busybox
	-$(MAKE) -C $(BUSYBOX_DIR) clean

busybox-dirclean:
	rm -rf $(BUSYBOX_DIR)
