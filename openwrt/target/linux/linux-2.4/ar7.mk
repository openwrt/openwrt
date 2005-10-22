#############################################################
# $Id$
#
# Makefile for the AR7-specific kernel/driver stuff
#
#############################################################

DOWNLOAD_SITE=http://openwrt.org/downloads/sources
# extracted from netgear DG834B V1.0.5 GPL release
ATM_FIRMWARE_DIR=sangam-atm-firmware-0.4
ATM_FIRMWARE_FILE=$(ATM_FIRMWARE_DIR).tar.gz
ATM_FIRMWARE_MD5SUM=8bfcb31109796502d66b11baaeb2fba6

$(DL_DIR)/$(ATM_FIRMWARE_FILE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(ATM_FIRMWARE_FILE) $(ATM_FIRMWARE_MD5SUM) $(DOWNLOAD_SITE) $(MAKE_TRACE)

$(LINUX_DIR)/.unpacked: $(DL_DIR)/$(ATM_FIRMWARE_FILE)
$(LINUX_DIR)/.depend_done: $(LINUX_DIR)/.drivers-unpacked
$(LINUX_DIR)/.modules_done: $(LINUX_DIR)/.drivers-unpacked

$(LINUX_DIR)/.drivers-unpacked: $(LINUX_DIR)/.unpacked
	-mkdir -p $(BUILD_DIR)
	zcat $(DL_DIR)/$(ATM_FIRMWARE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $@


$(eval $(call KMOD_template,SANGAM_ATM_A,sangam-atm-annex-a,\
	$(MODULES_DIR)/kernel/drivers/atm/tiatm.o \
,CONFIG_MIPS_SANGAM_ATM,kmod-atm,60,tiatm, \
	cp $(BUILD_DIR)/$(ATM_FIRMWARE_DIR)/ar0700xx_a.bin $$(I_SANGAM_ATM_A)/lib/modules/ar0700xx.bin \
))

$(eval $(call KMOD_template,SANGAM_ATM_B,sangam-atm-annex-b,\
	$(MODULES_DIR)/kernel/drivers/atm/tiatm.o \
,CONFIG_MIPS_SANGAM_ATM,kmod-atm,60,tiatm, \
	cp $(BUILD_DIR)/$(ATM_FIRMWARE_DIR)/ar0700xx_b.bin $$(I_SANGAM_ATM_B)/lib/modules/ar0700xx.bin \
))

$(eval $(call KMOD_template,CPMAC,cpmac,\
        $(MODULES_DIR)/kernel/drivers/net/avalanche_cpmac/avalanche_cpmac.o \
,CONFIG_MIPS_AVALANCHE_CPMAC))

