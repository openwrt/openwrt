#############################################################
#
# gettext
#
#############################################################
GETTEXT_SOURCE:=gettext-0.11.5.tar.gz
GETTEXT_SITE:=ftp://ftp.gnu.org/gnu/gettext
GETTEXT_DIR:=$(BUILD_DIR)/gettext-0.11.5
GETTEXT_CAT:=zcat
GETTEXT_BINARY:=gettext
GETTEXT_TARGET_BINARY:=usr/bin/gettext

$(DL_DIR)/$(GETTEXT_SOURCE):
	 $(WGET) -P $(DL_DIR) $(GETTEXT_SITE)/$(GETTEXT_SOURCE)

gettext-source: $(DL_DIR)/$(GETTEXT_SOURCE)

$(GETTEXT_DIR)/.unpacked: $(DL_DIR)/$(GETTEXT_SOURCE)
	$(GETTEXT_CAT) $(DL_DIR)/$(GETTEXT_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(GETTEXT_DIR)/.unpacked

$(GETTEXT_DIR)/.configured: $(GETTEXT_DIR)/.unpacked
	(cd $(GETTEXT_DIR); rm -rf config.cache; \
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
	);
	touch  $(GETTEXT_DIR)/.configured

$(GETTEXT_DIR)/$(GETTEXT_BINARY): $(GETTEXT_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(GETTEXT_DIR)

$(TARGET_DIR)/$(GETTEXT_TARGET_BINARY): $(GETTEXT_DIR)/$(GETTEXT_BINARY)
	$(MAKE) DESTDIR=$(STAGING_DIR) CC=$(TARGET_CC) -C $(GETTEXT_DIR) install
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(GETTEXT_DIR) install
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc \
		$(TARGET_DIR)/usr/doc

gettext: uclibc $(TARGET_DIR)/$(GETTEXT_TARGET_BINARY)

gettext-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(GETTEXT_DIR) uninstall
	-$(MAKE) -C $(GETTEXT_DIR) clean

gettext-dirclean:
	rm -rf $(GETTEXT_DIR)

