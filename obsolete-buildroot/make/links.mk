#############################################################
#
# links (text based web browser)
#
#############################################################
LINKS_SITE:=http://artax.karlin.mff.cuni.cz/~mikulas/vyplody/links/download/no-ssl
LINKS_SOURCE:=links-0.99pre9-no-ssl.tar.gz
LINKS_DIR:=$(BUILD_DIR)/links-0.99pre9-no-ssl

$(DL_DIR)/$(LINKS_SOURCE):
	$(WGET) -P $(DL_DIR) $(LINKS_SITE)/$(LINKS_SOURCE)

links-source: $(DL_DIR)/$(LINKS_SOURCE)

$(LINKS_DIR)/.unpacked: $(DL_DIR)/$(LINKS_SOURCE)
	zcat $(DL_DIR)/$(LINKS_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch  $(LINKS_DIR)/.unpacked

$(LINKS_DIR)/.configured: $(LINKS_DIR)/.unpacked
	(cd $(LINKS_DIR); rm -rf config.cache; \
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
		--localstatedir=/tmp \
		--mandir=/usr/man \
		--infodir=/usr/info \
		$(DISABLE_NLS) \
	);
	touch  $(LINKS_DIR)/.configured

$(LINKS_DIR)/links: $(LINKS_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(LINKS_DIR)
	$(STRIP) $(LINKS_DIR)/links

$(TARGET_DIR)/usr/bin/links: $(LINKS_DIR)/links
	install -c $(LINKS_DIR)/links $(TARGET_DIR)/usr/bin/links

links-clean: 
	$(MAKE) -C $(LINKS_DIR) clean

links-dirclean: 
	rm -rf $(LINKS_DIR) 

links: uclibc $(TARGET_DIR)/usr/bin/links

