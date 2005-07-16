#############################################################
# $Id$
#
# Makefile for the AR7-specific kernel/driver stuff
#
#############################################################

DOWNLOAD_SITE=http://openwrt.org/downloads/sources
# extracted from netgear DG834B V1.0.5 GPL release
ATM_FIRMWARE_DIR=sangam-atm-firmware-0.1
ATM_FIRMWARE_FILE=$(ATM_FIRMWARE_DIR).tar.gz
ATM_FIRMWARE_MD5SUM=dc1be257dcb536b6fa02a02c81956e7e

$(DL_DIR)/$(ATM_FIRMWARE_FILE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(ATM_FIRMWARE_FILE) $(ATM_FIRMWARE_MD5SUM) $(DOWNLOAD_SITE)

$(LINUX_DIR)/.unpacked: $(DL_DIR)/$(ATM_FIRMWARE_FILE)
$(LINUX_DIR)/.depend_done: $(LINUX_DIR)/.drivers-unpacked
$(LINUX_DIR)/.modules_done: $(LINUX_DIR)/.drivers-unpacked

$(LINUX_DIR)/.drivers-unpacked: $(LINUX_DIR)/.unpacked
	-mkdir -p $(BUILD_DIR)
	zcat $(DL_DIR)/$(ATM_FIRMWARE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@


$(eval $(call KMOD_template,SANGAM_ATM,sangam-atm,\
	$(MODULES_DIR)/kernel/drivers/atm/tiatm.o \
,CONFIG_MIPS_SANGAM_ATM,kmod-atm,60,tiatm, \
	cp $(BUILD_DIR)/$(ATM_FIRMWARE_DIR)/*.bin $$(I_SANGAM_ATM)/lib/modules/ \
))

$(eval $(call KMOD_template,CPMAC,cpmac,\
        $(MODULES_DIR)/kernel/drivers/net/avalanche_cpmac/avalanche_cpmac.o \
,CONFIG_MIPS_AVALANCHE_CPMAC))

