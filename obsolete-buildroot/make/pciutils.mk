#############################################################
#
# pciutils
#
#############################################################
PCIUTILS_SOURCE:=pciutils-2.1.10.tar.gz
PCIUTILS_SITE:=ftp://atrey.karlin.mff.cuni.cz/pub/linux/pci
PCIUTILS_DIR:=$(BUILD_DIR)/pciutils-2.1.10
PCIUTILS_CAT:=zcat

# Yet more targets...
PCIIDS_SITE:=http://pciids.sourceforge.net/
PCIIDS_SOURCE:=pci.ids.bz2
PCIIDS_CAT:=bzcat

$(DL_DIR)/$(PCIUTILS_SOURCE):
	 $(WGET) -P $(DL_DIR) $(PCIUTILS_SITE)/$(PCIUTILS_SOURCE)

$(DL_DIR)/$(PCIIDS_SOURCE):
	$(WGET) -P $(DL_DIR) $(PCIIDS_SITE)/$(PCIIDS_SOURCE)

pciutils-source: $(DL_DIR)/$(PCIUTILS_SOURCE) $(DL_DIR)/$(PCIIDS_SOURCE)

$(PCIUTILS_DIR)/.unpacked: $(DL_DIR)/$(PCIUTILS_SOURCE) $(DL_DIR)/$(PCIIDS_SOURCE)
	$(PCIUTILS_CAT) $(DL_DIR)/$(PCIUTILS_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	$(PCIIDS_CAT) $(DL_DIR)/$(PCIIDS_SOURCE) > $(PCIUTILS_DIR)/pci.id
	touch $(PCIUTILS_DIR)/.unpacked

$(PCIUTILS_DIR)/.configured: $(PCIUTILS_DIR)/.unpacked
	(cd $(PCIUTILS_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		$(DISABLE_NLS) \
	);
	touch  $(PCIUTILS_DIR)/.configured

$(PCIUTILS_DIR)/lspci: $(PCIUTILS_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(PCIUTILS_DIR)

$(TARGET_DIR)/sbin/lspci: $(PCIUTILS_DIR)/lspci
	install -c $(PCIUTILS_DIR)/lspci $(TARGET_DIR)/sbin/lspci

$(TARGET_DIR)/sbin/setpci: $(PCIUTILS_DIR)/setpci
	install -c $(PCIUTILS_DIR)/setpci $(TARGET_DIR)/sbin/setpci

$(TARGET_DIR)/usr/share/misc/pci.ids: $(PCIUTILS_DIR)/.dist
	install -Dc $(PCIUTILS_DIR)/pci.ids $(TARGET_DIR)/usr/share/misc/pci.ids


pciutils: uclibc $(TARGET_DIR)/sbin/setpci $(TARGET_DIR)/sbin/lspci $(TARGET_DIR)/usr/share/misc/pci.ids

pciutils-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(PCIUTILS_DIR) uninstall
	-$(MAKE) -C $(PCIUTILS_DIR) clean

pciutils-dirclean:
	rm -rf $(PCIUTILS_DIR)

