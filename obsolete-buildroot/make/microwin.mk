#############################################################
#
# Microwindows - 2003/11/17 Greg Haerr
# (requires CVS 2003/11/17 or later)
#
#############################################################
MICROWIN_SITE:=ftp://ftp.microwindows.org/pub/microwindows
MICROWIN_SOURCE:=microwindows-src-snapshot.tar.gz
MICROWIN_DIR:=$(BUILD_DIR)/microwin
#MICROWIN_SOURCE:=microwindows-0.91.tar.gz
#MICROWIN_DIR:=$(BUILD_DIR)/microwindows-0.91

MICROWIN_CAT:=zcat
MICROWIN_BINARY:=$(MICROWIN_DIR)/src/bin/nano-X
MICROWIN_TARGET_BINARY:=$(TARGET_DIR)/usr/bin/nano-X

MICROWIN_CONFIG:=$(MICROWIN_DIR)/src/Configs/config.uclibc

$(DL_DIR)/$(MICROWIN_SOURCE):
	 $(WGET) -P $(DL_DIR) $(MICROWIN_SITE)/$(MICROWIN_SOURCE)

microwin-source: $(DL_DIR)/$(MICROWIN_SOURCE)

$(MICROWIN_DIR)/.unpacked: $(DL_DIR)/$(MICROWIN_SOURCE)
	$(MICROWIN_CAT) $(DL_DIR)/$(MICROWIN_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(MICROWIN_DIR)/.unpacked

$(MICROWIN_DIR)/.configured: $(MICROWIN_DIR)/.unpacked
	(cd $(MICROWIN_DIR); \
	);
	touch  $(MICROWIN_DIR)/.configured

$(MICROWIN_BINARY): $(MICROWIN_DIR)/.configured
	$(MAKE) ARCH=LINUX-$(shell echo $(ARCH) | tr a-z A-Z) $(shell echo $(ARCH) | tr a-z A-Z)TOOLSPREFIX=$(TARGET_CROSS) CC=$(TARGET_CC) -C $(MICROWIN_DIR)/src CONFIG=$(MICROWIN_CONFIG)

$(MICROWIN_TARGET_BINARY): $(MICROWIN_BINARY)
	$(MAKE) INSTALL_PREFIX=$(TARGET_DIR)/usr INSTALL_OWNER1= INSTALL_OWNER2= CC=$(TARGET_CC) -C $(MICROWIN_DIR)/src CONFIG=$(MICROWIN_CONFIG) install

microwin: uclibc $(MICROWIN_TARGET_BINARY)

microwin-clean:
	-$(MAKE) -C $(MICROWIN_DIR)/src clean

microwin-dirclean:
	rm -rf $(MICROWIN_DIR)

