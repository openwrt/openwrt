#############################################################
#
# python
#
#############################################################
PYTHON_VERSION=2.3.2
PYTHON_SOURCE:=Python-$(PYTHON_VERSION).tgz
PYTHON_SITE:=http://python.org/ftp/python/$(PYTHON_VERSION)
PYTHON_DIR:=$(BUILD_DIR)/Python-$(PYTHON_VERSION)
PYTHON_CAT:=zcat
PYTHON_BINARY:=python
PYTHON_INSTALL_DIR:=$(BUILD_DIR)/python_install
PYTHON_TARGET_BINARY:=$(PYTHON_INSTALL_DIR)/bin/python


$(DL_DIR)/$(PYTHON_SOURCE):
	 $(WGET) -P $(DL_DIR) $(PYTHON_SITE)/$(PYTHON_SOURCE)

python-source: $(DL_DIR)/$(PYTHON_SOURCE)

$(PYTHON_DIR)/.unpacked: $(DL_DIR)/$(PYTHON_SOURCE)
	$(PYTHON_CAT) $(DL_DIR)/$(PYTHON_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(PYTHON_DIR)/.unpacked

$(PYTHON_DIR)/.configured: $(PYTHON_DIR)/.unpacked
	(cd $(PYTHON_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--sysconfdir=/etc \
		$(DISABLE_NLS) \
	);
	touch  $(PYTHON_DIR)/.configured

$(PYTHON_DIR)/$(PYTHON_BINARY): $(PYTHON_DIR)/.configured
	LD_LIBRARY_PATH=$(STAGING_DIR)/lib
	$(MAKE) CC=$(TARGET_CC) -C $(PYTHON_DIR)

$(TARGET_DIR)/$(PYTHON_TARGET_BINARY): $(PYTHON_DIR)/$(PYTHON_BINARY)

python: uclibc $(TARGET_DIR)/$(PYTHON_TARGET_BINARY)
	$(MAKE) CC=$(TARGET_CC) -C $(PYTHON_DIR) install
	rm $(PYTHON_INSTALL_DIR)/bin/idle
	rm $(PYTHON_INSTALL_DIR)/bin/pydoc
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

python-clean:
	-$(MAKE) -C $(PYTHON_DIR) distclean
	rm $(PYTHON_DIR)/.configured

python-dirclean:
	rm -rf $(PYTHON_DIR)

