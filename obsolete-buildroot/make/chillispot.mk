#############################################################
#
# chillispot
#
#############################################################

CHILLISPOT_VERSION:=0.95
CHILLISPOT_SOURCE:=chillispot-$(CHILLISPOT_VERSION).tar.gz
CHILLISPOT_SITE:=http://www.chillispot.org/download/
CHILLISPOT_CAT:=zcat
CHILLISPOT_DIR:=$(BUILD_DIR)/chillispot-$(CHILLISPOT_VERSION)
CHILLISPOT_BINARY:=src/chilli
CHILLISPOT_TARGET_BINARY:=usr/sbin/chilli

$(DL_DIR)/$(CHILLISPOT_SOURCE):
	 $(WGET) -P $(DL_DIR) $(CHILLISPOT_SITE)/$(CHILLISPOT_SOURCE)

chillispot-source: $(DL_DIR)/$(CHILLISPOT_SOURCE)

$(CHILLISPOT_DIR)/.unpacked: $(DL_DIR)/$(CHILLISPOT_SOURCE)
	$(CHILLISPOT_CAT) $(DL_DIR)/$(CHILLISPOT_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	sed -i -e s:"-ggdb"::g $(CHILLISPOT_DIR)/src/Makefile.in
	touch $(CHILLISPOT_DIR)/.unpacked

$(CHILLISPOT_DIR)/.configured: $(CHILLISPOT_DIR)/.unpacked
	(cd $(CHILLISPOT_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		ac_cv_func_setvbuf_reversed=no \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib/locate \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var/lib \
		--mandir=/usr/man \
		--infodir=/usr/info \
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
	);
	sed -i	-e s:"#define HAVE_MALLOC 0":"#define HAVE_MALLOC 1":g \
		-e s:"#define malloc rpl_malloc"::g \
		$(CHILLISPOT_DIR)/config.h
	touch  $(CHILLISPOT_DIR)/.configured

$(CHILLISPOT_DIR)/$(CHILLISPOT_BINARY): $(CHILLISPOT_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) $(TARGET_CONFIGURE_OPTS) -C $(CHILLISPOT_DIR) 

# This stuff is needed to work around GNU make deficiencies
chillispot-target_binary: $(CHILLISPOT_DIR)/$(CHILLISPOT_BINARY)
	@if [ -L $(TARGET_DIR)/$(CHILLISPOT_TARGET_BINARY) ] ; then \
		rm -f $(TARGET_DIR)/$(CHILLISPOT_TARGET_BINARY); fi;
	@if [ ! -f $(CHILLISPOT_DIR)/$(CHILLISPOT_BINARY) -o $(TARGET_DIR)/$(CHILLISPOT_TARGET_BINARY) \
	-ot $(CHILLISPOT_DIR)/$(CHILLISPOT_BINARY) ] ; then \
	    set -x; \
	    $(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(CHILLISPOT_DIR) install; \
	    $(STRIP) $(TARGET_DIR)/$(CHILLISPOT_TARGET_BINARY)
	    rm -rf $(TARGET_DIR)/usr/man; fi;
	    
chillispot: $(CHILLISPOT_DIR)/$(CHILLISPOT_BINARY)
	#chillispot-target_binary
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(CHILLISPOT_DIR) install
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(CHILLISPOT_DIR)/doc install
	$(STRIP) $(TARGET_DIR)/$(CHILLISPOT_TARGET_BINARY)
	

chillispot-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(CHILLISPOT_DIR) uninstall
	-$(MAKE) -C $(CHILLISPOT_DIR) clean

chillispot-dirclean:
	rm -rf $(CHILLISPOT_DIR)
